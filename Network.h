#ifndef NETWORK_H
#define NETWORK_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define WIFI_CONNECT_WAIT_MS 10000
#define MQTT_RECONNECT_INTERVAL_MS 5000
#define MQTT_DATA_SEND_INTERVAL_MS 10000

// High level status protocol messages
const uint8_t MSG_STATUS_OUT_READY = 0x52; //'R'
const char* MSG_STATUS_OUT_LAST_WILL = "L"; //null terminated 'L'
const uint8_t MSG_STATUS_IN_PING = 0x50; //P

struct SNetworkConfig {
  const char* wifiSsid;
  const char* wifiPassword;
  const char* mqttServerAddr;
  const uint16_t mqttServerPort;
  const char* mqttUser;
  const char* mqttPassword;
  const char* mqttClientId;
  const char* mqttTopicIn;
  const char* mqttTopicOut;
  const char* mqttTopicStatusIn;
  const char* mqttTopicStatusOut;
};

class CNetwork {
public:
  CNetwork(const SNetworkConfig config)
    : mNetworkConfig(config),
      mEspClient(),
      mMqttClient(mEspClient) {
    mMqttClient.setServer(mNetworkConfig.mqttServerAddr, mNetworkConfig.mqttServerPort);
    mMqttClient.setCallback(std::bind(&CNetwork::onMqttMsgReceived,
                                      this,
                                      std::placeholders::_1,
                                      std::placeholders::_2,
                                      std::placeholders::_3));
  }
  ~CNetwork() {
    
  }

  void loop() {
    unsigned long curTime = millis();
    if (WiFi.status() == WL_CONNECTED) {
      if (mMqttClient.connected()) {
        if (curTime - mTelemLastSendTime > MQTT_DATA_SEND_INTERVAL_MS) {
          telemetrySend();
          mTelemLastSendTime = curTime;
        }
        mMqttClient.loop();
      } else {
        if (curTime - mMqttLastConnectFailTime > MQTT_RECONNECT_INTERVAL_MS) {
          if (mqttConnect()) {
            mMqttClient.loop();
          } else {
            mMqttLastConnectFailTime = curTime;
          }
        }
      }
    } else {
      if (curTime - mWifiLastConnectTryTime > WIFI_CONNECT_WAIT_MS) {
        wifiConnect();
        mWifiLastConnectTryTime = curTime;
      }
    }
  }

  bool isConnected() {
    return WiFi.status() == WL_CONNECTED && mMqttClient.connected();
  }

private:
  void msgStatusSend(uint8_t msg) {
    mMqttClient.publish(mNetworkConfig.mqttTopicStatusOut, &msg, 1);
  }

  void msgSend(const char* msg) {
    mMqttClient.publish(mNetworkConfig.mqttTopicOut, msg);
  }

  bool mqttConnect() {
    if (mMqttClient.connect(mNetworkConfig.mqttClientId,
                            mNetworkConfig.mqttUser,
                            mNetworkConfig.mqttPassword,
                            mNetworkConfig.mqttTopicStatusOut,
                            0,
                            0,
                            MSG_STATUS_OUT_LAST_WILL)) {
      msgStatusSend(MSG_STATUS_OUT_READY);
      mMqttClient.subscribe(mNetworkConfig.mqttTopicStatusIn);
      mMqttClient.subscribe(mNetworkConfig.mqttTopicIn);
      return true;
    }
    return false;
  }

  void wifiConnect() {
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(mNetworkConfig.wifiSsid, mNetworkConfig.wifiPassword);
  }

  void telemetrySend() {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["temperature"] = gTemperature.getValue();

    size_t len = root.measureLength() + 1;
    char buffer[len];
    root.printTo(buffer, len);
    msgSend(buffer);
  }

  void onMqttMsgReceived(char* topic, byte* payload, unsigned int len) {
    // Status request
    if (!strcmp(topic, mNetworkConfig.mqttTopicStatusIn)) {
      if (len != 1) return;
      uint8_t cmd = (uint8_t)payload[0];
      switch (cmd) {
        case MSG_STATUS_IN_PING:
          msgStatusSend(MSG_STATUS_OUT_READY);
          break;

        default:
          break;
      }
    }
    // Control request
    else if (!strcmp(topic, mNetworkConfig.mqttTopicIn)) {
      // TODO
    }
  }

  SNetworkConfig mNetworkConfig;

  WiFiClient mEspClient;
  PubSubClient mMqttClient;

  unsigned long mWifiLastConnectTryTime = 0;
  unsigned long mMqttLastConnectFailTime = 0;
  unsigned long mTelemLastSendTime = 0;
};

extern CNetwork gNetwork;

#endif // NETWORK_H

