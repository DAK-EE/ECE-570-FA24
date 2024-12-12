#Author: Diego King
#Date: 12/10/2024
#Description: This program listens to a subscribed MQTT topic and prints the received message.
#If the message is a JSON it tries to parse it then push it to a mysql database. In this case
#the device running the python script happens to be the same device hosting the database (hence
#db_host = "localhost").

import paho.mqtt.client as mqtt
import mysql.connector
import simplejson as json

server = "broker.hivemq.com"
port = 1883
subscribe_topic = "myesp/testing/room"

db_host = "localhost"
db_user = "DiegoKing"
db_pass = "****"
db_database = "db_DiegoKing"

def on_connect(client, userdata, flags,rc):
	if rc == 0:
		print("Connected to MQTT broker.")
		client.subscribe(subscribe_topic)
		print(f"Subscribed to topic: {subscribe_topic}")
	else:
		print(f"Failed to connect, return code {rc}")

def on_message(client, userdata, msg):
	rawdata = msg.payload.decode()
	print(f"Received message: {rawdata}")
	try:
		data = json.loads(rawdata)
		value = data['status'][0]['ADC']
		try:
			connection = mysql.connector.connect(
				host = db_host,
				user = db_user,
				password = db_pass,
				database = db_database
			)
			if connection.is_connected():
				print("Connected to database!")
				cursor = connection.cursor()
				insert_query = "INSERT INTO mqtt_esp (data) VALUES (%s)"
				cursor.execute(insert_query, (value,))
				connection.commit()
				print(f"Value {value} inserted into mqtt_esp table.")
		except mysql.connector.Error as err:
			print(f"Error: {err}")
		finally:
			if connection.is_connected():
				cursor.close()
				connection.close()
				print("Database connection closed.")
	except:
		pass
client = mqtt.Client(transport = "TCP")

client.on_connect = on_connect
client.on_message = on_message

print("Connecting to broker...")
client.connect(server, port, 60)
client.loop_forever()
