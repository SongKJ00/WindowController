package com.example.skj.windowcontroller;

import android.app.TimePickerDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TimePicker;
import android.widget.Toast;

import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {
    static final int REQUEST_ENABLE_BT = 10;    //블루투스 활성 상태의 변경 결과를 앱으로 알려줄 때의 식별자
    static final int MOTION_OPEN = 1;   //open 버튼 클릭 시의 모션
    static final int MOTION_CLOSE = -1; //close 버튼 클릭 시의 모션
    static final int MOTION_NONE = 0;
    static final byte END = 'E';
    int mPariedDeviceCount = 0; //페어링된 디바이스 개수
    int readBufferPosition; //수신 데이터 버퍼에 저장 인덱스
    int windowMotion = 0;    //창문에 준 신호가 open or close 신호 판별

    Set<BluetoothDevice> mDevices;  //페어링 디바이스 집합
    BluetoothAdapter mBluetoothAdapter; //블루투스 모듈 사용 위한 오브젝트
    BluetoothDevice mRemoteDevice; //연결하고자 하는 원격 블루투스 기기
    BluetoothSocket mSocket = null; //블루투스 소켓
    OutputStream mOutputStream = null;  //데이터 전송 스트림
    InputStream mInputStream = null;    //데이터 수신 스트림
    Thread mWorkerThread;   //데이터 수신용 스레드

    char mCharDelimiter = '\n'; //character 데이터 전용 분류 문자
    String mStrDelimiter = "\n"; //String 데이터 전용 분류 String
    Button timeSetBtn;
    Button deviceSearchBtn;
    boolean deviceConnected = false;    //device 연결 상태
    boolean actionEnded = true;    //창문 action 종료 상태
    String spName = "setTimeData";  //SharedPreferences name

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        //if(mBluetoothAdapter == null)
        //{
         //   System.out.println("No support bluetooth");
        //    finish();
        //}

        //else
         //   if(!mBluetoothAdapter.isEnabled())
         //   {
         //       Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
         //       startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
         //   }
        //fragment1 = new Fragment1();
        //fragment2 = new Fragment2();
       // Button powerButton = (Button)findViewById(R.id.powerButton);
        //Button timeButton = (Button)findViewById(R.id.timeButton);
        //powerButton.setOnClickListener(new OnClickListener() {
        //    public void onClick(View v){
         //       getSupportFragmentManager().beginTransaction().replace(R.id.container, fragment1).commit();
         //   }
       // });*/

        //timeButton.setOnClickListener(new OnClickListener() {
         //   public void onClick(View v){
         //       getSupportFragmentManager().beginTransaction().replace(R.id.container, fragment2).commit();
        //        new TimePickerDialog(MainActivity.this, timeSetListener, 15, 24, false).show();
        //
        // }
        //});*/
        //현재 시간 정보 얻어오기
        java.util.Calendar calendar = java.util.Calendar.getInstance();
        final int hour = calendar.get(calendar.HOUR_OF_DAY);
        final int minute = calendar.get(calendar.MINUTE);

        //시간 설정 버튼 터치 시 현재 시간으로 기본 세팅
        timeSetBtn = (Button)findViewById(R.id.timeSetBtn);
        timeSetBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(deviceConnected == true) {
                    new TimePickerDialog(MainActivity.this, timeSetListener, hour, minute + 1, false).show();
                }
                else {
                    Toast.makeText(getApplicationContext(), "연결된 디바이스가 없습니다.", Toast.LENGTH_SHORT).show();
                }
            }
        });

        //deviceSearchBtn 터치 시 블루투스 설정 창 출력
        deviceSearchBtn = (Button)findViewById(R.id.deviceSearchBtn);
        deviceSearchBtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                checkBluetooth();
            }
        });
    }

    //블루투스 사용 가능 체킹 및 디바이스 선택 메소드 호출
    void checkBluetooth() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(mBluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(), "기기가 블루투스를 지원하지 않습니다", Toast.LENGTH_LONG).show();
            finish();
        }

        else{
            if(!mBluetoothAdapter.isEnabled()) {
                Toast.makeText(getApplicationContext(), "현재 블루투스가 비활성 상태입니다", Toast.LENGTH_LONG).show();
                Intent enableBTIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBTIntent, REQUEST_ENABLE_BT);
            }
            else
                selectDevice();
        }
    }


    @Override
    //블루투스 OFF시 ON/OFF 설정 창 출력
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch(requestCode) {
            case REQUEST_ENABLE_BT:
                if (resultCode == RESULT_OK) {
                    selectDevice();
                }
                break;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    //페어링된 장치 리스트 출력 및 장치 선택
    void selectDevice() {
        mDevices = mBluetoothAdapter.getBondedDevices();
        mPariedDeviceCount = mDevices.size();

        if(mPariedDeviceCount == 0)
            Toast.makeText(getApplicationContext(), "페어링된 장치가 없습니다.", Toast.LENGTH_SHORT ).show();
        else {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("블루투스 장치 선택");

            List<String> listItems = new ArrayList<String>();
            for(BluetoothDevice device : mDevices) {
                listItems.add(device.getName());
            }
            listItems.add("취소");

            final CharSequence[] items = listItems.toArray(new CharSequence[listItems.size()]);
            builder.setItems(items, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int item) {
                    //취소를 누르지 않은 경우; 취소를 누르면 item 값은 0
                    if(item != mPariedDeviceCount) {
                        connectToSelectedDevice(items[item].toString());
                    }
                }
            });

            builder.setCancelable(false);
            AlertDialog alert = builder.create();
            alert.show();
        }
    }

    //선택한 장치에 연결하기
    void connectToSelectedDevice(String selectedDeviceName) {
        mRemoteDevice = getDeviceFromBondedList(selectedDeviceName);
        UUID uuid = mRemoteDevice.getUuids()[0].getUuid();
        try {
            //window 관련 블루투스 장치이면
            if(mRemoteDevice.getName().contains("window")) {
                mSocket = mRemoteDevice.createRfcommSocketToServiceRecord(uuid);
                Method m = mRemoteDevice.getClass().getMethod("createInsecureRfcommSocket", new Class[]{int.class});    //이부분은 잘 모르겠음
                mSocket = (BluetoothSocket) m.invoke(mRemoteDevice, 1);
                mBluetoothAdapter.cancelDiscovery();
                mSocket.connect();

                mOutputStream = mSocket.getOutputStream();
                mInputStream = mSocket.getInputStream();
                deviceConnected = true;
                Toast.makeText(getApplicationContext(), "디바이스와 연결되었습니다.", Toast.LENGTH_SHORT).show();
                recieveData();
            }
            else {
                Toast.makeText(getApplicationContext(), "연결 가능한 디바이스가 아닙니다.", Toast.LENGTH_SHORT).show();
            }
        } catch (Exception e) {
                 Log.d("hello world!!!!!!!", "connectToSelectedDevice: "+ e.getMessage());
           Toast.makeText(getApplicationContext(), "블루투스 연결 중 오류가 발생했습니다.", Toast.LENGTH_SHORT).show();
        }
    }

    //선택된 디바이스 알아내기
    BluetoothDevice getDeviceFromBondedList(String name) {
        BluetoothDevice selectedDevice = null;
        for(BluetoothDevice device : mDevices) {
            if(name.equals(device.getName())) {
                selectedDevice = device;
                break;
            }
        }
        return selectedDevice;
    }

    //데이터 수신
    void recieveData() {
        //Toast.makeText(getApplication(), "recieveData", Toast.LENGTH_SHORT).show();
        //System.out.println("Hello");
        final Handler handler = new Handler();
        final byte[] readBuffer = new byte[1024];
        readBufferPosition = 0;

        mWorkerThread = new Thread(){
            @Override
            public void run() {
               // System.out.println("Hello");
                while(!Thread.currentThread().isInterrupted()) {
                    try {
                        int byteAvailable = mInputStream.available();
                        if(byteAvailable > 0) {
                            //System.out.println("received");
                            //Toast.makeText(getApplicationContext(), "I got it", Toast.LENGTH_SHORT).show();\
                            //byte[] packetBytes = new byte[byteAvailable];
                            //mInputStream.read(packetBytes);
                            //for(int i = 0; i < byteAvailable; i++) {
                            //    byte b = packetBytes[i];
                            //    if(b == mCharDelimiter) {
                            //        byte[] encodedBytes = new byte[readBufferPosition];
                            //        System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                            //        final String data = new String(encodedBytes, "US-ASCII");
                            //        readBufferPosition = 0;
                            final byte[] receivedData = new byte[byteAvailable];    //수신 데이터 담을 공간
                            mInputStream.read(receivedData);
                                    //데이터 수신 시의 행동
                                    handler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            //창문이 다 닫혔다고 시그널이 오면
                                            if(receivedData[0] == END) {
                                                //open 버튼이 터치되면
                                                if(windowMotion == MOTION_OPEN) {
                                                    Toast.makeText(getApplicationContext(), "창문이 열렸습니다.", Toast.LENGTH_SHORT).show();
                                                }
                                                //close 버튼이 터치되면
                                                else if(windowMotion == MOTION_CLOSE) {
                                                    Toast.makeText(getApplicationContext(), "창문이 닫혔습니다.", Toast.LENGTH_SHORT).show();
                                                }
                                                actionEnded = true; //액션이 끝났으므로 다음 액션 enable
                                                windowMotion = MOTION_NONE; //toast창 이후 모션 none으로 설정
                                            }
                                            else {
                                                //Toast.makeText(getApplicationContext(), "제대로 된 데이터가 수신되지 않았습니다.", Toast.LENGTH_SHORT).show();
                                            }
                                            //actionEnded = true;
                                            //Toast.makeText(getApplicationContext(), "데이터가 수신되었습니다", Toast.LENGTH_SHORT).show();
                                            //else if(data.equals("open\n")) {
                                            //    Toast.makeText(getApplicationContext(), "창문이 열렸습니다.", Toast.LENGTH_SHORT);
                                            //}
                                            //else if(data.equals("connected")) {
                                            //   receivedData = data;
                                            //}
                                        }
                                    });
                                }
                                //else {
                                //    readBuffer[readBufferPosition++] = b;
                                //    System.out.println("fuck");
                                //}
                            }catch(Exception e) {
                        Toast.makeText(getApplicationContext(), "데이터 수신 중 오류가 발생했습니다.", Toast.LENGTH_SHORT).show();
                    }
                }
            }
        };
        mWorkerThread.start();

    }

    //데이터 송신
    void sendData(byte[] msg) {
        //msg += mStrDelimiter;
        try {
            for(int i = 0; i < msg.length; i++) {
                mOutputStream.write(msg[i]);
                try {
                    Thread.sleep(200);
                }catch(Exception e){
                    Toast.makeText(getApplicationContext(), "에러 발생!!", Toast.LENGTH_SHORT).show();
                }
            }
        }catch(Exception e) {
            Toast.makeText(getApplicationContext(), "데이터 전송 중 오류가 발생했습니다.", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    //앱 종료 시 각종 object close
    protected void onDestroy() {
        try{
            mWorkerThread.interrupt();
            mInputStream.close();
            //mOutputStream.close();
            mSocket.close();
        }catch(Exception e) { }
        super.onDestroy();
    }

    private TimePickerDialog.OnTimeSetListener timeSetListener = new TimePickerDialog.OnTimeSetListener() {
        //TimePicker 확인 버튼 터치 시
        @Override
        public void onTimeSet(TimePicker timePicker, int hour, int minute) {
            //오전 오후 설정
            String AM_PM = new String();
            String timeData = new String();
            //timeData = "T" + Integer.toString(hour) + " : " + "5" + "3"; //Integer.toString(minute);
            //System.out.println(timeData);
            //byte[] msg = timeData.getBytes();
            //Toast.makeText(getApplicationContext(), timeData, Toast.LENGTH_SHORT).show();

            //오전 오후 판별
            if(hour >= 12) {
                AM_PM = "오후";
                if(hour > 13) {
                    hour = hour - 12;
                }
            }
            else {
                AM_PM = "오전";
                if(hour == 0) {
                    hour = hour + 12;
                }
            }

            //설정된 시간 전송이라는 것을 알리기 위해 T를 첨가
            //시나 분이 한 자리수라면 디자인적을 감안해 앞에 0을 붙임
            if(hour / 10 == 0) {
                if(minute / 10 == 0) {
                    timeData = "T" + "0" + Integer.toString(hour) + ":" + "0" + Integer.toString(minute) +  (AM_PM.equals("오전") ? "AM" : "PM");
                }
                else {
                    timeData = "T" + "0" + Integer.toString(hour) + ":" + Integer.toString(minute) + (AM_PM.equals("오전") ? "AM" : "PM");
                }
            }
            else {
                if(minute / 10 == 0) {
                    timeData = "T" + Integer.toString(hour) + ":" + "0" + Integer.toString(minute) + (AM_PM.equals("오전") ? "AM" : "PM");
                }
                else {
                    timeData = "T" + Integer.toString(hour) + ":" + Integer.toString(minute) +  (AM_PM.equals("오전") ? "AM" : "PM");
                }
            }

            sendData(timeData.getBytes());  //설정 시간 데이터 전송
            Toast.makeText(getApplicationContext(), AM_PM + " " + hour + "시 " + minute + "분으로 설정되었습니다.", Toast.LENGTH_SHORT).show();
            SharedPreferences sp = getSharedPreferences(spName, 0);
            SharedPreferences.Editor editor = sp.edit();
            editor.putString("hour", String.valueOf(hour));
            editor.putString("minute", String.valueOf(minute));
            editor.commit();
        }
    };

    //open 버튼 누를 때
    public void onOpenClicked(View v) {
        if(deviceConnected == true) {
            //if(connectedRightDevice() == true) {
            if(actionEnded == true) {
                actionEnded = false;
                windowMotion = MOTION_OPEN;
                byte[] msg = new byte[1024];
                msg[0] = 'O';
                sendData(msg);
                Toast.makeText(getApplicationContext(), "창문이 열리는 중입니다.", Toast.LENGTH_SHORT).show();
            }
            else if(windowMotion == MOTION_OPEN){
                Toast.makeText(getApplicationContext(), "아직 창문이 열리는 중입니다.", Toast.LENGTH_SHORT).show();
            }
            else {
                Toast.makeText(getApplicationContext(), "아직 창문이 닫히는 중입니다.", Toast.LENGTH_SHORT).show();
            }
            //}
            //else {
            //    Toast.makeText(getApplicationContext(), "사용 가능한 디바이스가 아닙니다.", Toast.LENGTH_SHORT).show();
            //}
        }
        else {
            Toast.makeText(getApplicationContext(), "연결된 디바이스가 없습니다.", Toast.LENGTH_SHORT).show();
        }
    }

    //close 버튼 누를 때
    public void onCloseClicked(View v) {
        if(deviceConnected == true) {
            //if(connectedRightDevice() == true) {
            if(actionEnded == true) {
                actionEnded = false;
                windowMotion = MOTION_CLOSE;
                byte[] msg = new byte[1024];
                msg[0] = 'C';
                sendData(msg);
                Toast.makeText(getApplicationContext(), "창문이 닫히는 중입니다.", Toast.LENGTH_SHORT).show();
            }
            else if(windowMotion == MOTION_CLOSE){
                Toast.makeText(getApplicationContext(), "아직 창문이 닫히는 중입니다.", Toast.LENGTH_SHORT).show();
            }
            else {
                Toast.makeText(getApplicationContext(), "아직 창문이 열리는 중입니다.", Toast.LENGTH_SHORT).show();
            }
            //}
            //else {
               // Toast.makeText(getApplicationContext(), "사용 가능한 디바이스가 아닙니다.", Toast.LENGTH_SHORT).show();
            //}
        }
        else {
            Toast.makeText(getApplicationContext(), "연결된 디바이스가 없습니다.", Toast.LENGTH_SHORT).show();
        }
    }
    //public boolean connectedRightDevice() {
    //    if (receivedData.equals("connected")) {
    //        return true;
    //    } else {
    //        return false;
    //   }
    //}
    //checksavedtime 버튼 눌렀을 때
    public void onCheckSavedTimeClicked(View v) {
        SharedPreferences sp = getSharedPreferences(spName, 0); //저장된 시간 값 가져오기
        Integer hour = new Integer(Integer.parseInt(sp.getString("hour", "")));
        Integer minute = new Integer(Integer.parseInt(sp.getString("minute", "")));
        String AM_PM = hour.intValue() >= 12 ? "오후" : "오전";
        //설정된 시각이 없을 경우
        if(hour == null || minute == null) {
            Toast.makeText(getApplicationContext(), "설정된 시간이 없습니다.", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(getApplicationContext(), "설정된 시간은 " + AM_PM + " " + hour.intValue() + "시 " + minute.intValue() + "분입니다.", Toast.LENGTH_SHORT).show();
        }
    }
}
