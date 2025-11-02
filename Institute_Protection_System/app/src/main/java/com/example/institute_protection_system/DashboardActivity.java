package com.example.institute_protection_system;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.util.Log;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import android.os.Handler;
import org.json.JSONObject;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import java.io.IOException;

public class DashboardActivity extends AppCompatActivity {

    private static final String SENSOR_DATA_URL = "https://institute-protection-system-backend.onrender.com/sensor-data";

    private TextView tvTemperature, tvGasLevel, tvLight, tvFireStatus, switchLed, switchFan;
    private Handler handler;
    private Runnable updateRunnable;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dashboard);

        tvTemperature = findViewById(R.id.tv_temperature);
        tvGasLevel = findViewById(R.id.tv_gas_level);
        tvLight = findViewById(R.id.tv_light);
        tvFireStatus = findViewById(R.id.tv_fire_status);
        switchLed = findViewById(R.id.switch_led);
        switchFan = findViewById(R.id.switch_fan);

        handler = new Handler();
        updateRunnable = new Runnable() {
            @Override
            public void run() {
                fetchSensorData();
                handler.postDelayed(this, 3000);
            }
        };
        handler.post(updateRunnable);
    }
    private void fetchSensorData() {
        OkHttpClient client = new OkHttpClient();

        Request request = new Request.Builder()
                .url(SENSOR_DATA_URL)
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                runOnUiThread(() -> {
                    Toast.makeText(DashboardActivity.this, "Failed to fetch data", Toast.LENGTH_SHORT).show();
                    Log.e("SensorData", "Error fetching data", e);
                });
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (response.isSuccessful()) {
                    try {
                        String responseBody = response.body().string();
                        JSONObject jsonResponse = new JSONObject(responseBody);

                        String temperature = jsonResponse.optString("temperature", "N/A");
                        String gasLevel = jsonResponse.optString("gas_level", "N/A");
                        String light = jsonResponse.optString("light_intensity", "N/A");
                        String fireStatus = jsonResponse.optString("fire_detected", "N/A");
                        String fanStatus = jsonResponse.optString("fan_status", "N/A");
                        String ledstatus = jsonResponse.optString("led_status", "N/A");

                        runOnUiThread(() -> {
                            tvTemperature.setText(temperature+"C");
                            tvGasLevel.setText(gasLevel+"%");
                            if(light.equals("true")){
                                tvLight.setText("LOW");
                            }
                            else {
                                tvLight.setText("HIGH");
                            }

                            if(fireStatus.equals("true")){
                                tvFireStatus.setText("YES");
                            }
                            else{
                                tvFireStatus.setText("NO");
                            }

                            if(fanStatus.equals("true")){
                                switchFan.setText("ON");
                            }
                            else {
                                switchFan.setText("OFF");
                            }
                            if(ledstatus.equals("true")){
                                switchLed.setText("ON");
                            }
                            else {
                                switchLed.setText("OFF");
                            }
                        });
                    } catch (Exception e) {
                        Log.e("SensorData", "Error parsing JSON response", e);
                    }
                } else {
                    runOnUiThread(() -> Toast.makeText(DashboardActivity.this, "Request failed", Toast.LENGTH_SHORT).show());
                }
            }
        });
    }
}