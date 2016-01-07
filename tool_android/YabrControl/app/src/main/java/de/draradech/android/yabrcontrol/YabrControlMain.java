package de.draradech.android.yabrcontrol;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothSocket;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Button;


public class YabrControlMain extends AppCompatActivity implements SensorEventListener
{
    private SensorManager mSensorManager;
    private Button mConnectButton;
    private Button mRemoteButton;
    private Button mAutonomyButton;
    private Button mPowerStartButton;
    private Timer mTimer;
    private TimerTask mTimerTask;
    private BluetoothSocket mBluetoothSocket;
    private Runnable mRunnable;
    private OutputStream mOut;
    private float mV1, mV2, mV3;
    private boolean mRemoteEnabled;
    private boolean mAutonomyEnabled;
    private boolean mConnected;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(de.draradech.android.yabrcontrol.R.layout.activity_main);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mConnectButton = (Button) findViewById(de.draradech.android.yabrcontrol.R.id.connect_button);
        mRemoteButton = (Button) findViewById(de.draradech.android.yabrcontrol.R.id.remote_button);
        mAutonomyButton = (Button) findViewById(de.draradech.android.yabrcontrol.R.id.autonomy_button);
        mPowerStartButton = (Button) findViewById(de.draradech.android.yabrcontrol.R.id.powerstart_button);
        mAutonomyEnabled = true;
        mTimer = new Timer();
        mRunnable = new Runnable(){@Override public void run(){tick();}};
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        try
        {
            mBluetoothSocket = BluetoothAdapter.getDefaultAdapter().getRemoteDevice("00:06:66:02:CE:27").createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
            Log.i("YabrControl", "socket created");
        }
        catch(IOException ioe)
        {
            Log.w("YabrControl", "socket creation failed: " + ioe.toString());
        }
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_GAME);
        mTimerTask = new TimerTask(){@Override public void run(){runOnUiThread(mRunnable);}};
        mTimer.scheduleAtFixedRate(mTimerTask, 0, 100);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        mSensorManager.unregisterListener(this);
        mTimerTask.cancel();
    }

    @Override
    public void onSensorChanged(SensorEvent event)
    {
        mV1 = event.values[0];
        mV2 = event.values[1];
        mV3 = event.values[2];
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy)
    {
    }

    private void tick()
    {
        if(mRemoteEnabled)
        {
            try
            {
                mOut.write('{');
                mOut.write('f');
                mOut.write((int)(mV3*10));
                mOut.write('}');
                mOut.write('{');
                mOut.write('l');
                int rl = (int)(mV2*20);
                mOut.write(rl<-126?-126:rl>126?126:rl);
                mOut.write('}');
            }
            catch(IOException ioe)
            {
                Log.w("YabrControl", "write failed: " + ioe.toString());
            }
        }
    }

    public void connect(View v)
    {
        if(mConnected)
        {
            try
            {
                mBluetoothSocket.close();
                mRemoteEnabled = false;
                mRemoteButton.setEnabled(false);
                mRemoteButton.setText("Enable Remote");
                mAutonomyButton.setEnabled(false);
                mPowerStartButton.setEnabled(false);
                mConnectButton.setText("Connect");
                mConnected = false;
                Log.i("YabrControl", "disconnected");
                mBluetoothSocket = BluetoothAdapter.getDefaultAdapter().getRemoteDevice("00:06:66:02:CE:27").createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
                Log.i("YabrControl", "new socket created");
            }
            catch(IOException ioe)
            {
                Log.w("YabrControl", "disconnect failed: " + ioe.toString());
            }
        }
        else
        {
            try
            {
                mBluetoothSocket.connect();
                mConnectButton.setText("Disconnect");
                Log.i("YabrControl", "connected!");
                mRemoteButton.setEnabled(true);
                mAutonomyButton.setEnabled(true);
                mPowerStartButton.setEnabled(true);
                mOut = mBluetoothSocket.getOutputStream();
                mOut.write('{');
                mOut.write('s');
                mOut.write('}');
                mConnected = true;
                Log.i("YabrControl", "silence requested");
            }
            catch(IOException ioe)
            {
                Log.w("YabrControl", "connect failed: " + ioe.toString());
                try
                {
                    mBluetoothSocket = BluetoothAdapter.getDefaultAdapter().getRemoteDevice("00:06:66:02:CE:27").createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
                    Log.i("YabrControl", "new socket created");
                }
                catch(IOException ioe2)
                {
                    Log.w("YabrControl", "new socket creation failed: " + ioe2.toString());
                }
            }
        }
    }

    public void powerstart(View v)
    {
        if(!mConnected) return;
        try
        {
            mOut.write('{');
            mOut.write('h');
            mOut.write('}');
        }
        catch(IOException ioe)
        {
            Log.w("YabrControl", "couldn't send powerstart: " + ioe.toString());
        }

    }
    public void autonomy(View v)
    {
        if(!mConnected) return;
        try
        {
            if(mAutonomyEnabled)
            {
                mAutonomyEnabled = false;
                mAutonomyButton.setText("Enable Autonomy");
                mOut.write('{');
                mOut.write('g');
                mOut.write('|');
                mOut.write(0x20);
                mOut.write('}');
            }
            else
            {
                mAutonomyEnabled  = true;
                mAutonomyButton.setText("Disable Autonomy");
                mOut.write('{');
                mOut.write('g');
                mOut.write(1);
                mOut.write('}');
            }
        }
        catch(IOException ioe)
        {
            Log.w("YabrControl", "couldn't send autonomy mode: " + ioe.toString());
        }
    }
    public void remote(View v)
    {
        if(!mConnected) return;
        if(mRemoteEnabled)
        {
            mRemoteEnabled = false;
            mRemoteButton.setText("Enable Remote");
        }
        else
        {
            mRemoteEnabled  = true;
            mRemoteButton.setText("Disable Remote");
        }
    }
}