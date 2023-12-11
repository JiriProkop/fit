<?php

require('vendor/autoload.php');

use \PhpMqtt\Client\MqttClient;
use \PhpMqtt\Client\ConnectionSettings;

$MQTT_SEND_DATA_TOPIC = "/lightsense-xproko47/data";
$MQTT_GET_THREASHOLD_TOPIC = "/lightsense-xproko47/set_threshold";


$server   = 'broker.hivemq.com';
$port     = 1883;
$clientId = rand(5, 15);
$username = 'jezo';
$password = 'public';
$clean_session = false;
$mqtt_version = MqttClient::MQTT_3_1_1;

$connectionSettings = (new ConnectionSettings)
  ->setUsername($username)
//   ->setPassword($password)
  ->setKeepAliveInterval(60)
//   ->setLastWillTopic('emqx/test/last-will')
  ->setLastWillMessage('client disconnect')
  ->setLastWillQualityOfService(1);

  $mqtt = new MqttClient($server, $port, $clientId, $mqtt_version);

$mqtt->connect($connectionSettings, $clean_session);
printf("client connected\n");

$mqtt->subscribe($MQTT_SEND_DATA_TOPIC, function ($topic, $message) {
    printf("Received message on topic [%s]: %s\n", $topic, $message);
}, 0);

//$mqtt->publish($MQTT_GET_THREASHOLD_TOPIC, $payload, 0, true);

$mqtt->loop(true);
