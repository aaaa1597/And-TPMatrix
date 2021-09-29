#ifndef TESTNATIVESURFACE_H
#define TESTNATIVESURFACE_H

#include <jni.h>
#include <android/native_window.h>
#include <pthread.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "Matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_test_tpdrw_NativeFunc2_create(JNIEnv *env, jclass type, jint id);
JNIEXPORT void JNICALL Java_com_test_tpdrw_NativeFunc2_surfaceCreated(JNIEnv *env, jclass type, jint id, jobject surface);
JNIEXPORT void JNICALL Java_com_test_tpdrw_NativeFunc2_surfaceChanged(JNIEnv *env, jclass type, jint id, jint width, jint height);
JNIEXPORT void JNICALL Java_com_test_tpdrw_NativeFunc2_surfaceDestroyed(JNIEnv *env, jclass type, jint id);

#ifdef __cplusplus
}
#endif

class FujiDmyPcl {
public:
	void createProgram();
	void preDraw();
	void draw();

private:
	const char *PCL_V_SHADER =
		"attribute vec4 vPclPosition;\n"
		"void main() {\n"
		"    gl_Position = vPclPosition;\n"
		"}\n";

	const char *PCL_F_SHADER =
		"precision mediump float;\n"
		"void main() {\n"
		"    gl_FragColor = vec4(0.0, 0.0, 1.0, 0.5);\n"
		"}\n";

private:
	GLuint mProgram = -1;
	Matrix::GPointf3 srcLines[6*2+28*2] = {0};

private:
	void createProgram(const char *vertexshader, const char *fragmentshader);
	GLuint loadShader(int shadertype, const char *vertexshader);
	void checkGlError(const char *argstr);

};

class CppSurfaceView2 {
private:
    const char *VERTEXSHADER =
        "attribute vec4 vPosition;\n"
        "uniform mat4 u_rotMatrix;\n"
        "void main() {\n"
        "    gl_Position = u_rotMatrix * vPosition;\n"
        "}\n";

    const char *FRAGMENTSHADER =
        "precision mediump float;\n"
        "void main() {\n"
        "    gl_FragColor = vec4(0.0, 1.0, 1.0, 0.2);\n"
        "}\n";

    const char *LINEVSHADER =
        "attribute vec4 vLinePosition;\n"
        "void main() {\n"
        "    gl_Position = vLinePosition;\n"
        "}\n";

    const char *LINEFSHADER =
        "precision mediump float;\n"
        "void main() {\n"
        "    gl_FragColor = vec4(1.0, 0.0, 0.0, 0.5);\n"
        "}\n";

    /* 移動 */
    static const int AMOUNTOFMOVE = -5;
    float mMoveX = AMOUNTOFMOVE;
    float mMoveY = AMOUNTOFMOVE;
    float mxPos = 100;
    float myPos = 130;
    GLuint mu_rotMatrixHandle = -1;

private:
    GLuint createProgram(int seqno, const char *vertexshader, const char *fragmentshader);
    GLuint loadShader(int shadertype, const char *vertexshader);
    void checkGlError(const char *argstr);

public:
    static const int STATUS_NONE   = 0;
    static const int STATUS_INITIALIZING = 1;
    static const int STATUS_DRAWING= 2;
    static const int STATUS_FINISH = 3;
    int mId = -1;
    int mStatus = STATUS_NONE;
    pthread_t mThreadId = -1;
    ANativeWindow *mWindow = NULL;
    EGLDisplay mEGLDisplay = NULL;
    EGLContext mEGLContext = NULL;
    EGLSurface mEGLSurface = NULL;
    GLuint mProgram = -1;
    GLuint mLineProgram = -1;
    bool isSurfaceCreated = false;
    int DspW = 0;
    int DspH = 0;

	FujiDmyPcl *pFujiDmyPcl;

public:
    CppSurfaceView2(int id);
    virtual ~CppSurfaceView2();
    static void *draw_thread(void *pArg);
    void createThread(JNIEnv *pEnv, jobject surface);
    void initEGL();
    void finEGL();
    void initGL();
    void predrawGL();
    void drawGL();
    void destroy();
};

#endif //TESTNATIVESURFACE_H
