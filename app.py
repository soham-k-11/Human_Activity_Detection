from flask import Flask,request,jsonify
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras.models import load_model
import numpy as np

buffered_data = []
activities = [
    "Walking Forward",
    "Walking Left",
    "Walking Right",
    "Walking Upstairs",
    "Walking Downstairs",
    "Running Forward",
    "Jumping Up",
    "Sitting",
    "Standing",
    "Sleeping",
    "Elevator Up",
    "Elevator Down"
]


app = Flask(__name__)
model = load_model('lstm_model_no_scale.keras')

@app.route("/Init", methods=["GET"])
def Init():
    global buffered_data
    buffered_data = []
    return "Buffer Cleared."


@app.route("/predict", methods=["POST"])
def predict():
    global buffered_data
    try:
        data = (request.get_json(force=True))

        if not isinstance(data, list) or not all(isinstance(row, list) and len(row) == 6 for row in data):
            return jsonify({"error": "Invalid input format"}), 400

        buffered_data.extend(np.array(data))

        if len(buffered_data) < 300:
            return jsonify({"Response":"Waiting for more..."}), 200

        if len(buffered_data) > 300:
            buffered_data = []  
            return jsonify({"error": "Too many rows received. Buffer reset."}), 400
        
        features = np.array(buffered_data).reshape(1, 300, 6)
        prediction = model.predict(features).argmax()
        buffered_data = []
        return activities[int(prediction)]   

    except Exception as e:
        return jsonify({"error": str(e)}), 400
    
@app.route("/", methods=["GET"])
def home():
    return "ML API is running!"

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000,debug=True)