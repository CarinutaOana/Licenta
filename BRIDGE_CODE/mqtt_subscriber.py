import paho.mqtt.client as mqtt
import os
from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()

# Configuration from environment
broker_address = os.getenv("MQTT_BROKER")
port = int(os.getenv("MQTT_PORT", "1883"))
username = os.getenv("MQTT_USER")
password = os.getenv("MQTT_PASS")
topic = [
    ("esp32_2/temperature", 0),
    ("esp32_2/humidity", 0),
    ("esp32_2/pressure", 0),
    ("esp32_2/gas", 1),
    ("esp32_2/gas/alarm", 2)
]

def on_connect(client, userdata, flags, rc):
    print("Connected with result code", rc)
    client.subscribe(topic)

def on_message(client, userdata, message):
    print(f"Received message on topic {message.topic}: {message.payload.decode()}")

client = mqtt.Client()
client.username_pw_set(username, password)

client.tls_set()
client.tls_insecure_set(True)

client.on_connect = on_connect
client.on_message = on_message

client.connect(broker_address, port, keepalive=60)
client.loop_forever()
