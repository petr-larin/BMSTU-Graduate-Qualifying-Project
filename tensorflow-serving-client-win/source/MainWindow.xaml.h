// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::tfs_client::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void button_predict_click(IInspectable const& sender, RoutedEventArgs const& args);
        void button_clear_click(IInspectable const& sender, RoutedEventArgs const& args);

        void ContactServer(bool get_prediction);

        void slider_f1_onchanged(IInspectable const& sender, RoutedEventArgs const& args);
        void numbox_f1_onchanged(IInspectable const& sender, NumberBoxValueChangedEventArgs const& args);
        void slider_f4_onchanged(IInspectable const& sender, RoutedEventArgs const& args);
        void numbox_f4_onchanged(IInspectable const& sender, NumberBoxValueChangedEventArgs const& args);
        void slider_f7_onchanged(IInspectable const& sender, RoutedEventArgs const& args);
        void numbox_f7_onchanged(IInspectable const& sender, NumberBoxValueChangedEventArgs const& args);

        void ClearPredictions();
    };

    HWND GetWin32Hwnd(MainWindow const& ui3_wnd);
    void HideTitleBarIcon(HWND win32_hwnd);
    double GetDisplayScaleFactorX();
    double GetDisplayScaleFactorY();
    void SetIcon(HWND win32_hwnd);
    void MyTimerProc(HWND, UINT, UINT_PTR This, DWORD);
    void MinMaxScaler_transform(double& f1, double& f4, double& f7);
    void MinMaxScaler_inverse_transform(double& f2, double& f3, double& f5, double& f6, double& rul);
}

namespace winrt::tfs_client::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
