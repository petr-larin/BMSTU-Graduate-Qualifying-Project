## Graduate Qualifying Project for the Data Science course at BMSTU in spring 2023 (11784DS)

This is a work in progress - check back soon! The final exam is on May 1st 2023.

###

In this grad project, I study the [RUL dataset](https://github.com/ignavinuales/Battery_RUL_Prediction) created by Ignacio Viñuales.

### Results

This is the covariation matrix before and after preprocessing:

![Covariation matrices](readme-images/cov_matrices.jpg)

This shows how classic regressors sometimes perform better than a neural network:

###

![R2 criterion](readme-images/R2.jpg)

###

This is a diagram of my dense neural network trained on the RUL dataset:

###

![TensorFlow Serving](readme-images/tfs.jpg)

###

This is a Windows desktop client for the neural network. I used Microsoft's WinUI3 framework and it is really cool. I also used the wonderful [cpr - C++ Requests library](https://docs.libcpr.org/) and C++20 [std::format](https://en.cppreference.com/w/cpp/utility/format/format) function which are both very helpful for those with Python background.

###

![Screen shot](readme-images/tfs_client.jpg)

