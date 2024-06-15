package korea.media.hw3;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Button;
import android.widget.ToggleButton;
import android.view.Display;
import android.graphics.Point;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    GLSurfaceRenderer glSurfaceRenderer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /*
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        */

        // create OpenGL ES surface view
        GLSurfaceView glSurfaceView = new GLSurfaceView(this);
        glSurfaceView.setEGLContextClientVersion(2);
        glSurfaceRenderer = new GLSurfaceRenderer();
        glSurfaceView.setRenderer(glSurfaceRenderer);

        setContentView(glSurfaceView);
        final ToggleButton button1 = new ToggleButton(this);
//        final ToggleButton button2 = new ToggleButton(this);

        button1.setText("Stop Moving Head");
        button1.setTextOff("Move Head");
        button1.setTextOn("Stop Moving Head");
//        button2.setText("Lower");
//        button2.setTextOff("Move Lower");
//        button2.setTextOn("Stop Lower");

        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        int width = size.x;
        int height = size.y;
        int buttonWidth  = width/8;
        int buttonHeight = buttonWidth;

        button1.setWidth(buttonWidth);
        button1.setHeight(buttonHeight);

//        button2.setWidth(buttonWidth);
//        button2.setHeight(buttonHeight);

        button1.setX(width/2 - buttonWidth);
        button1.setY(7*(height-buttonHeight)/8);

//        button2.setX(width/2 + buttonHeight );
//        button2.setY(7*(height-buttonHeight)/8);


        button1.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if(button1.isChecked())
                {
                    setButtonFlag(false);
                }
                else
                {
                    setButtonFlag(true);
                }
            }
        });

//        button2.setOnClickListener(new View.OnClickListener()
//        {
//            @Override
//            public void onClick(View v)
//            {
//                if(button2.isChecked())
//                {
//                    setLowerFlag(false);
//                }
//                else
//                {
//                    setLowerFlag(true);
//                }
//            }
//        });

        ViewGroup.LayoutParams lp = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        this.addContentView(button1, lp);
//        this.addContentView(button2, lp);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void surfaceCreated(AssetManager assetManager);
    public native void surfaceChanged(int width, int height);
    public native void drawFrame(float deltaTime);
    public native void setButtonFlag(boolean upperFlag);
    private class GLSurfaceRenderer implements GLSurfaceView.Renderer {

        // last time attribute for calculating delta time
        private long lastTime = System.currentTimeMillis();

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // call native method
            surfaceCreated(getAssets());
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            // call native method
            surfaceChanged(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            // calculate delta time
            long currentTime = System.currentTimeMillis();
            float deltaTime = (float) (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;

            // call native method
            drawFrame(deltaTime);
        }
    }
}
