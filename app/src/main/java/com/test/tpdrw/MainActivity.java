package com.test.tpdrw;

import androidx.appcompat.app.AppCompatActivity;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.widget.TextView;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SurfaceView surfacefull = ((SurfaceView)findViewById(R.id.surfacefull));
        /* 透過設定 */
        surfacefull.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        surfacefull.setZOrderOnTop(true);
        /* コールバック設定 */
        surfacefull.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                /* C++ */
                NativeFunc2.create(0);
                NativeFunc2.surfaceCreated(0, holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                NativeFunc2.surfaceChanged(0, width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                NativeFunc2.surfaceDestroyed(0);
            }
        });

        SurfaceView surface = ((SurfaceView)findViewById(R.id.surface));
        /* 透過設定 */
        surface.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        surface.setZOrderOnTop(true);
        /* コールバック設定 */
        surface.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                /* C++ */
                NativeFunc.create(0);
                NativeFunc.surfaceCreated(0, holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                NativeFunc.surfaceChanged(0, width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                NativeFunc.surfaceDestroyed(0);
            }
        });
    }
}
