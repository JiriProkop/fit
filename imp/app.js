
const brokerUrl = 'wss://broker.emqx.io:8084/mqtt';
const topicToSubscribe = '/lightsense-xproko47/data';
const thresholdTopic = '/lightsense-xproko47/set_threshold';

const options = {
    // Clean session
    clean: true,
    connectTimeout: 4000,
    // Authentication
    clientId: 'emqx_test',
    username: 'emqx_test',
  }
// Create an MQTT client instance
const client = mqtt.connect(brokerUrl, options);


// Set up the callback for when the client connects to the broker
client.on('connect', function () {
  // Subscribe to the desired topic
  client.subscribe(topicToSubscribe);
});

// Set up the callback for when a message is received
client.on('message', function (topic, message) {
  // Check if the received message is for the subscribed topic
  if (topic === topicToSubscribe) {
    // Update the HTML with the received message
    document.getElementById('message').innerHTML = 'Received Message: ' + message.toString();
  }
});

// Function to publish the threshold value
function publishThreshold() {
  const thresholdValue = document.getElementById('thresholdInput').value;
  
  // Publish the threshold value to the 'threshold' topic
  client.publish(thresholdTopic, thresholdValue);
}
