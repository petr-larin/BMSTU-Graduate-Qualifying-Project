// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::tfs_client::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        Title(L"TensorFlow Serving Client : : RUL Dataset");

        auto win32_hwnd = GetWin32Hwnd(*this);

        SetIcon(win32_hwnd);  // remove in MSIX version

        RECT cliRect{}, wndRect{};
        GetClientRect(win32_hwnd, &cliRect);
        GetWindowRect(win32_hwnd, &wndRect);

        SetWindowPos(win32_hwnd, NULL, 0, 0,
            int(630 * GetDisplayScaleFactorX()) + wndRect.right - wndRect.left - cliRect.right,
            int(383 * GetDisplayScaleFactorY()) + wndRect.bottom - wndRect.top - cliRect.bottom,
            SWP_NOMOVE);

        numbox_f1().Value(1500); // Initial values - somewhere around median
        numbox_f4().Value(3.5);  //
        numbox_f7().Value(8000); //

        SetTimer(win32_hwnd, UINT_PTR(this), 4000, MyTimerProc);
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t)
    {
        throw hresult_not_implemented();
    }

    void MainWindow::button_predict_click(IInspectable const&, RoutedEventArgs const&)
    {
        ContactServer(true);
    }

    void MainWindow::button_clear_click(IInspectable const&, RoutedEventArgs const&)
    {
        ClearPredictions();
    }

    void MainWindow::slider_f1_onchanged(IInspectable const&, RoutedEventArgs const&)
    {
        numbox_f1().Value(slider_f1().Value());
    }

    void MainWindow::numbox_f1_onchanged(IInspectable const&, NumberBoxValueChangedEventArgs const&)
    {
        auto f1_value = numbox_f1().Value();
        if (isnan(f1_value))
        {
            f1_value = 100.0;
            numbox_f1().Value(f1_value);
        }

        slider_f1().Value(f1_value);
    }

    void MainWindow::slider_f4_onchanged(IInspectable const&, RoutedEventArgs const&)
    {
        numbox_f4().Value(slider_f4().Value());
    }

    void MainWindow::numbox_f4_onchanged(IInspectable const&, NumberBoxValueChangedEventArgs const&)
    {
        auto f4_value = numbox_f4().Value();
        if (isnan(f4_value))
        {
            f4_value = 3.0;
            numbox_f4().Value(f4_value);
        }

        slider_f4().Value(f4_value);
    }

    void MainWindow::slider_f7_onchanged(IInspectable const&, RoutedEventArgs const&)
    {
        numbox_f7().Value(slider_f7().Value());
    }

    void MainWindow::numbox_f7_onchanged(IInspectable const&, NumberBoxValueChangedEventArgs const&)
    {
        auto f7_value = numbox_f7().Value();
        if (isnan(f7_value))
        {
            f7_value = 2500.0;
            numbox_f7().Value(f7_value);
        }

        slider_f7().Value(f7_value);
    }

    void MainWindow::ClearPredictions()
    {
        textbox_f2().Text(L"");
        textbox_f3().Text(L"");
        textbox_f5().Text(L"");
        textbox_f6().Text(L"");
        textbox_rul().Text(L"");
    }

    HWND GetWin32Hwnd(MainWindow const& ui3_wnd)
    {
        auto windowNative{ ui3_wnd.try_as<::IWindowNative>() };
        winrt::check_bool(windowNative);
        HWND hWnd{ 0 };
        windowNative->get_WindowHandle(&hWnd);
        return hWnd;
    }

    void HideTitleBarIcon(HWND win32_hwnd)
    {
        auto ext_style = GetWindowLongPtrW(win32_hwnd, GWL_EXSTYLE) | WS_EX_DLGMODALFRAME;
        SetWindowLongPtrW(win32_hwnd, GWL_EXSTYLE, ext_style);
    }

    double GetDisplayScaleFactorX()
    {
        auto desktopDc = GetDC(NULL);
        if (desktopDc == NULL) return 1.0;
        
        auto horizontalDPI = GetDeviceCaps(desktopDc, LOGPIXELSX);
        return double(horizontalDPI) * 25.0 / 24.0 / 100.0;
    }

    double GetDisplayScaleFactorY()
    {
        auto desktopDc = GetDC(NULL);
        if (desktopDc == NULL) return 1.0;

        auto verticalDPI = GetDeviceCaps(desktopDc, LOGPIXELSY);
        return double(verticalDPI) * 25.0 / 24.0 / 100.0;
    }

    void MainWindow::ContactServer(bool get_prediction)
    {
        using json = nlohmann::json;

        std::string body = R"({})"; // Empty request to bypass prediction stage

        if (get_prediction)
        {
            double f1 = numbox_f1().Value();
            double f4 = numbox_f4().Value();
            double f7 = numbox_f7().Value();

            MinMaxScaler_transform(f1, f4, f7);

            json inst;
            inst["instances"] = { {f1, f4, f7} };
            body = inst.dump();
        }

        auto response = cpr::Post(cpr::Url{ "http://77.37.180.59:8501/v1/models/Battery_RUL:predict" },
            cpr::Body{ body },
            cpr::Header{ { "Content-Type", "application/json" } },
            cpr::Timeout{ 500 }
        );

        using namespace std::literals;

        hstring status_msg{
            response.status_code == 200 || response.status_code == 400 ?
            L"Connected to 77.37.180.59:8501"sv : L"Contacting the server..."sv };

        NetworkStatusMsg().Text(status_msg);

        if (get_prediction)
            if (response.status_code == 200)
            {
                auto pred = json::parse(response.text)["predictions"][0];
                
                double f2 = pred[0];
                double f3 = pred[1];
                double f5 = pred[2];
                double f6 = pred[3];
                double rul = pred[4];

                MinMaxScaler_inverse_transform(f2, f3, f5, f6, rul);
                                
                textbox_f2().Text(std::format(L"{:.2f}", f2));
                textbox_f3().Text(std::format(L"{:.3f}", f3));
                textbox_f5().Text(std::format(L"{:.2f}", f5));
                textbox_f6().Text(std::format(L"{:.2f}", f6));
                textbox_rul().Text(std::format(L"{:.2f}", rul));
            }
            else
            {
                ClearPredictions();
                textbox_f2().Text(L"Unavailable");
            }

        if (!get_prediction)
            if (response.status_code == 400)
                if (textbox_f2().Text() == L"Unavailable")
                {
                    textbox_f2().Text(L"");
                }
    }

    void MyTimerProc(HWND, UINT, UINT_PTR This, DWORD)
    {
        ((MainWindow*)(This))->ContactServer(false);
    }


    void SetIcon(HWND win32_hwnd)
    {
        auto windowId = GetWindowIdFromWindow(win32_hwnd);
        auto appWindow = AppWindow::GetFromWindowId(windowId);

        auto iconHandle = LoadImageW(GetModuleHandleW(nullptr),
            MAKEINTRESOURCEW(110), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);

        auto iconId = winrt::Microsoft::UI::GetIconIdFromIcon(HICON(iconHandle));

        appWindow.SetIcon(iconId);

        //SendMessageW(win32_hwnd, WM_SETICON, ICON_SMALL, LPARAM(NULL));
        //SendMessageW(win32_hwnd, WM_SETICON, ICON_BIG, LPARAM(iconHandle));
    }

    void MinMaxScaler_transform(double& f1, double& f4, double& f7)
    {
        //
        // exported from Jupyter to min_max_for_c++.txt:
        // 
        // X = ['F1', 'F4', 'F7']
        // app_MMS_X.data_min_ = [168.0, 3.206, 3509.92]
        // app_MMS_X.data_max_ = [8414.98, 4.257, 14986.88]
        //

        constexpr std::array<double, 3> MMS_min = { 168.0, 3.206, 3509.92 };
        constexpr std::array<double, 3> MMS_max = { 8414.98, 4.257, 14986.88 };

        auto normalize = [=](double& arg, int ind)
        {
            arg = (arg - MMS_min[ind]) / (MMS_max[ind] - MMS_min[ind]);
        };

        normalize(f1, 0);
        normalize(f4, 1);
        normalize(f7, 2);
    }

    void MinMaxScaler_inverse_transform(double& f2, double& f3, double& f5, double& f6, double& rul)
    {
        //
        // exported from Jupyter to min_max_for_c++.txt:
        // 
        // y = ['F2', 'F3', 'F5', 'F6', 'RUL']
        // app_mms_y.data_min_ = [144.6785714291036, 3.65, -70.7626667059958, 44.32, 1.0]
        // app_mms_y.data_max_ = [1435.0080000000053, 4.255, 5508.993000000002, 13780.12, 1133.0]
        //

        constexpr std::array<double, 5> MMS_min = { 144.6785714291036, 3.65, -70.7626667059958, 44.32, 1.0 };
        constexpr std::array<double, 5> MMS_max = { 1435.0080000000053, 4.255, 5508.993000000002, 13780.12, 1133.0 };

        auto denormalize = [=](double& arg, int ind)
        {
            arg = arg * (MMS_max[ind] - MMS_min[ind]) + MMS_min[ind];
        };

        denormalize(f2, 0);
        denormalize(f3, 1);
        denormalize(f5, 2);
        denormalize(f6, 3);
        denormalize(rul, 4);
    }
}

