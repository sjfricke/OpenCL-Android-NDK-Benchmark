package com.spencerfricke.opencl_ndk_benchmark;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {

    static final String TAG = "OpenCL_Benchmark";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("opencl_benchmark");
    }

    Button mStartTestButton;
    SurfaceView mSurfaceView;
    SurfaceHolder mSurfaceHolder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        onCreateJNI(getAssets());

        // set up the Surface to display images too
        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceView);
        mSurfaceHolder = mSurfaceView.getHolder();

        mStartTestButton = (Button)findViewById(R.id.start_test);
        mStartTestButton.setOnClickListener(startTestListener);

        mSurfaceHolder.addCallback(new SurfaceHolder.Callback() {

            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.v(TAG, "surfaceChanged format=" + format + ", width=" + width + ", height="
                        + height);
            }

            public void surfaceCreated(SurfaceHolder holder) {
                Log.v(TAG, "surfaceCreated");
                setSurface(holder.getSurface());
            }

            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.v(TAG, "surfaceDestroyed");
            }

        });
    }

    private View.OnClickListener startTestListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.result_text);
        tv.setText(startTest());
        }
    };

    public native void onCreateJNI(AssetManager assetManager);

    public native String startTest();

    // Sends surface buffer to NDK
    public native void setSurface(Surface surface);
}
