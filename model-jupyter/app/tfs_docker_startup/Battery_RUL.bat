docker run -t --rm -p 8501:8501 -v "c:/tensorflow_serving/saved_models/Battery_RUL:/models/Battery_RUL" -e MODEL_NAME=Battery_RUL tensorflow/serving


