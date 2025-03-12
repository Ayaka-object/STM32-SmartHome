package com.example.aliyun_mqtt;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {
    private Handler handler;
    private MqttClient client;
    private final String host = "host";
    private final String userName = "userName";
    private final String passWord = "passWord";
    private final String ClientId = "ClientID";
    private final String mqtt_sub_topic = "topic";//订阅话题
    private final String mqtt_pub_topic = "topic";//发布话题
    private MqttConnectOptions mqttConnectOptions;

    private TextView temp;
    private TextView humi;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView MQTT_state = findViewById(R.id.MQTT_state);
        temp = findViewById(R.id.temperature_textview);
        humi = findViewById(R.id.humidity_textview);
        Mqtt_init();
        startReconnect();
        handler = new Handler(Looper.myLooper()) {//用于处理
            @SuppressLint("SetTextI18n")
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1: //开机校验更新回传
                        break;
                    case 2:  // 反馈回传
                        break;
                    case 3:  //MQTT 收到消息回传
                        System.out.println(msg.obj.toString());   // 显示MQTT数据
                        break;
                    case 31:   //连接成功
                        MQTT_state.setText("连接成功");
                        try {
                            client.subscribe(mqtt_sub_topic, 1);//订阅
                        } catch (MqttException e) {
                            e.printStackTrace();
                        }
                        break;
                    case 30:  //连接失败
                        Toast.makeText(MainActivity.this, "连接失败", Toast.LENGTH_SHORT).show();
                        MQTT_state.setText("连接失败");
                        break;
                    default:
                        break;
                }
            }
        };

        Switch switch_LED = findViewById(R.id.switch_LED);
        switch_LED.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                publish_message_plus(mqtt_pub_topic,"{\"COMMAND\":\"LED_ON\"}");
            } else {
                publish_message_plus(mqtt_pub_topic,"{\"COMMAND\":\"LED_OFF\"}");
            }
        });

        Switch switch_Motor = findViewById(R.id.switch_Motor);
        switch_Motor.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    publish_message_plus(mqtt_pub_topic,"{\"COMMAND\":\"Motor_ON\"}");
                } else {
                    publish_message_plus(mqtt_pub_topic,"{\"COMMAND\":\"Motor_OFF\"}");
                }
            }
        });

    }

    private void Mqtt_init() {
        try {
            client = new MqttClient(host, ClientId, new MemoryPersistence());
            //MQTT的连接设置
            mqttConnectOptions = new MqttConnectOptions();
            mqttConnectOptions.setCleanSession(false);
            mqttConnectOptions.setUserName(userName);
            mqttConnectOptions.setPassword(passWord.toCharArray());
            mqttConnectOptions.setConnectionTimeout(10);

            // 设置会话心跳时间 单位为秒 服务器会每隔1.5*30秒的时间向客户端发送个消息判断客户端是否在线，但这个方法并没有重连的机制
            //由于自身网络延时很难确定，建议设大一点，防止断开连接后无法重连
            mqttConnectOptions.setKeepAliveInterval(30);
            //设置回调
            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    //连接丢失后，一般在这里面进行重连
                    System.out.println("connectionLost----------");
                    startReconnect();
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    //publish后会执行到这里
                    System.out.println("deliveryComplete---------"
                            + token.isComplete());
                }

                @Override
                public void messageArrived(String topicName, MqttMessage message)
                        throws Exception {
                    //subscribe后得到的消息会执行到这里面
                    System.out.println("messageArrived----------");
                    parseJson(String.valueOf(message));
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    // MQTT连接函数
    private void Mqtt_connect() {
        new Thread(new Runnable() {
            @Override
            public void run() {

                try {
                    if (!client.isConnected())  //如果还未连接
                    {
                        client.connect(mqttConnectOptions);
                        Message msg = new Message();
                        msg.what = 31;
                        handler.sendMessage(msg);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    e.getMessage();
                    Message msg = new Message();
                    msg.what = 30;
                    handler.sendMessage(msg);
                }

            }
        }).start();
    }

    // MQTT重新连接函数
    private void startReconnect() {
        ScheduledExecutorService scheduler = Executors.newSingleThreadScheduledExecutor();
        scheduler.scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                if (!client.isConnected()) {
                    Mqtt_connect();
                }
            }
        },0, 10 * 1000, TimeUnit.MILLISECONDS);
    }

    // 订阅函数    (下发任务/命令)
    private void publish_message_plus(String topic, String message2) {
        if (client == null || !client.isConnected()) {
            return;
        }
        MqttMessage message = new MqttMessage();
        message.setPayload(message2.getBytes());
        message.setQos(1);//设置消息质量
        //MQTT一共有三种消息质量
        //Qos0:会发生消息的丢失或重复
        //Qos1:至少送达一次
        //Qos2：保证只送达到目标端一次，网络开销最高
        try {
            client.publish(topic, message);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void parseJson(String jsonString) {
        try {
            // 将字符串转换为 JSONObject
            JSONObject jsonObject = new JSONObject(jsonString);

            // 提取 temp 对象
            JSONObject tempObject = jsonObject.getJSONObject("temp");
            int temperature = tempObject.getInt("value");

            JSONObject humidityObject = jsonObject.getJSONObject("humidity");
            int humidity = humidityObject.getInt("value");

            System.out.println(temperature);
            System.out.println(humidity);

            runOnUiThread(() -> {
                temp.setText(temperature + "");
                humi.setText(humidity + "");
            });

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }




}
