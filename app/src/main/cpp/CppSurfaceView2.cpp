#include <stdlib.h>
#include <map>
#include <jni.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include "CppSurfaceView2.h"
#include "Matrix.h"

std::map<int, CppSurfaceView2*> gpSufacesLists2;

#ifdef __cplusplus
extern "C" {
#endif

void Java_com_test_tpdrw_NativeFunc2_create(JNIEnv *pEnv, jclass type, jint id) {
    gpSufacesLists2[id] = new CppSurfaceView2(id);
}

void Java_com_test_tpdrw_NativeFunc2_surfaceCreated(JNIEnv *pEnv, jclass type, jint id, jobject surface) {
    gpSufacesLists2[id]->createThread(pEnv, surface);
}

void Java_com_test_tpdrw_NativeFunc2_surfaceChanged(JNIEnv *pEnv, jclass type, jint id, jint width, jint height) {
    gpSufacesLists2[id]->isSurfaceCreated = true;
    gpSufacesLists2[id]->DspW = width;
    gpSufacesLists2[id]->DspH = height;
    __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "[%d](DspW,DspH)=(%d,%d)", id,gpSufacesLists2[id]->DspW, gpSufacesLists2[id]->DspH);
}

void Java_com_test_tpdrw_NativeFunc2_surfaceDestroyed(JNIEnv *pEnv, jclass type, jint id) {
    gpSufacesLists2[id]->mStatus = CppSurfaceView2::STATUS_FINISH;
    gpSufacesLists2[id]->destroy();
}

#ifdef __cplusplus
}
#endif

/**************/
/* FujiDmyPcl */
/**************/
void FujiDmyPcl::createProgram() {
	createProgram(PCL_V_SHADER, PCL_F_SHADER);
	return;
}
void FujiDmyPcl::createProgram(const char *vertexshader, const char *fragmentshader) {
	GLuint vhandle = loadShader(GL_VERTEX_SHADER, vertexshader);
	if(vhandle == GL_FALSE) __android_log_assert("","","");

	GLuint fhandle = loadShader(GL_FRAGMENT_SHADER, fragmentshader);
	if(vhandle == GL_FALSE) __android_log_assert("","","");

	mProgram = glCreateProgram();
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa FujiDmyPcl::mProgram=%d", mProgram);
	if(mProgram == GL_FALSE) {
		checkGlError("glCreateProgram");
		__android_log_assert("","","");
	}

	glAttachShader(mProgram, vhandle);
	checkGlError("glAttachShader(VERTEX_SHADER)");
	glAttachShader(mProgram, fhandle);
	checkGlError("glAttachShader(FRAGMENT_SHADER)");

	glLinkProgram(mProgram);
	GLint linkStatus = GL_FALSE;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &linkStatus);
	if(linkStatus != GL_TRUE) {
		GLint bufLen = 0;
		glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &bufLen);
		if(bufLen) {
			char *logstr = (char*)malloc(bufLen);
			glGetProgramInfoLog(mProgram, bufLen, NULL, logstr);
			__android_log_print(ANDROID_LOG_ERROR, "FujiDmyPcl", "glLinkProgram() Fail!!\n %s", logstr);
			free(logstr);
		}
		glDeleteProgram(mProgram);
		mProgram = GL_FALSE;
	}

	/* 後処理 */
	glDeleteShader(vhandle);
	glDeleteShader(fhandle);

	return;
}

GLuint FujiDmyPcl::loadShader(int shadertype, const char *sourcestring) {
	GLuint shaderhandle = glCreateShader(shadertype);
	if(!shaderhandle) return GL_FALSE;

	glShaderSource(shaderhandle, 1, &sourcestring, NULL);
	glCompileShader(shaderhandle);

	GLint compiled = GL_FALSE;
	glGetShaderiv(shaderhandle, GL_COMPILE_STATUS, &compiled);
	if(!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shaderhandle, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen) {
			char *logbuf = (char*)malloc(infoLen);
			if(logbuf) {
				glGetShaderInfoLog(shaderhandle, infoLen, NULL, logbuf);
				__android_log_print(ANDROID_LOG_ERROR, "FujiDmyPcl", "shader failuer!!\n%s", logbuf);
				free(logbuf);
			}
		}
		glDeleteShader(shaderhandle);
		shaderhandle = GL_FALSE;
	}

	return shaderhandle;
}

void FujiDmyPcl::checkGlError(const char *argstr) {
	for(GLuint error = glGetError(); error; error = glGetError())
		__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView2", "after %s errcode=%d", argstr, error);
}

void FujiDmyPcl::preDraw() {
	/* -49152 to 61440 */
	int lpct = 0;
	for(int ypos=-4096; lpct < 6; lpct++,ypos+=4096) {
		srcLines[lpct*2].x = -49152;
        if(ypos==-4096) srcLines[lpct*2].y = -3000;
        else srcLines[lpct*2].y = ypos;
		srcLines[lpct*2].z = 0;
		srcLines[lpct*2+1].x = 61440;
        if(ypos==-4096) srcLines[lpct*2+1].y = -3000;
        else srcLines[lpct*2+1].y = ypos;
		srcLines[lpct*2+1].z = 0;
	}
	for(int xpos=-49152; lpct < 6+28; lpct++,xpos+=4096) {
		srcLines[lpct*2].x = xpos;
		srcLines[lpct*2].y = 16384;
		srcLines[lpct*2].z = 0;
		srcLines[lpct*2+1].x = xpos;
		srcLines[lpct*2+1].y = -100;
		srcLines[lpct*2+1].z = 0;
	}

	return;
}

static float cnt = 0;
void FujiDmyPcl::draw() {
//    Matrix::GPointf3 srcaaa[12] = {{0,4096,0},{4096,4096,0},{4096,0,0},{0,0,0},{4096,8192,0},{8192,8192,0},{8192,4096,0},{4096,4096,0},{8192,12288,0},{12288,12288,0},{12288,8192,0},{8192,8192,0}};
//    Matrix::GPointf3 dstaaa[12] = {0};
    Matrix::GPointf3 srcbbb[6*2+28*2] = {0};
    memcpy(srcbbb, srcLines, sizeof(srcLines));
    Matrix::GPointf3 dstbbb[6*2+28*2] = {0};

    /* 俯角変換行列生成 */
    float viewpoint2 = 1950;
    float tmpPerM[4][4] = {0};
    Matrix::LoadIdentity(tmpPerM);
    Matrix::TranslateMatrix(6144,6144,0,tmpPerM);
    Matrix::PerspectiveMatrix2(viewpoint2, tmpPerM);
    Matrix::TranslateMatrix(-6144,-6144,0,tmpPerM);
    float tmpRotM[4][4] = {0};
    Matrix::LoadIdentity(tmpRotM);
    Matrix::RotateMatrix(cnt<90?cnt+=0.1:cnt=0, 1, 0, 0, tmpRotM);
    const float tmpTransPoseM[4][4] = {tmpRotM[0][0],tmpRotM[1][0],tmpRotM[2][0],tmpRotM[3][0], tmpRotM[0][1],tmpRotM[1][1],tmpRotM[2][1],tmpRotM[3][1], tmpRotM[0][2],tmpRotM[1][2],tmpRotM[2][2],tmpRotM[3][2], tmpRotM[0][3],tmpRotM[1][3],tmpRotM[2][3],tmpRotM[3][3]};
    Matrix::MultiMatrix(tmpTransPoseM, tmpPerM);
    /* 俯角変換行列適用 */
//    Matrix::ConvVertexf3(12, srcaaa, (const float(*)[4])tmpPerM, dstaaa);
    Matrix::ConvVertexf3(sizeof(srcbbb)/sizeof(Matrix::GPointf3), srcbbb, (const float(*)[4])tmpPerM, dstbbb);

    /* Fiji変換行列(正規化4096→ドット座標)生成 */
    float FujiM[4][4];
    Matrix::LoadIdentity(FujiM);
//    Matrix::ScaleMatrix(1, -1, 1, FujiM);                                   /* y軸反転 */
    Matrix::ScaleMatrix(1/1.1f/* AspectW */, 1/1.f/* AspectH */, 0, FujiM); /* アスペクト比適用 */
//    Matrix::RotateMatrix(10, 0, 0, 1, FujiM);                             /* 回転 */
    Matrix::ScaleMatrix(800/* PclDotSize.w *//4096.f, 650/* PclDotSize.h *//4096.f, 0, FujiM);   /* 正規化→ドット座標 */
//    Matrix::TranslateMatrix(0, 720, 0, FujiM);                              /* y軸反転移動 */

    /* Fiji変換行列適用 */
//    Matrix::ConvVertexf3(sizeof(srcLines)/sizeof(Matrix::GPointf3), srcLines, (const float(*)[4])FujiM, dstLines);
//    memcpy(srcaaa, dstaaa, sizeof(srcaaa));
//    Matrix::ConvVertexf3(12, srcaaa, (const float(*)[4])FujiM, dstaaa);
    memcpy(srcbbb, dstbbb, sizeof(srcbbb));
    Matrix::ConvVertexf3(sizeof(srcbbb)/sizeof(Matrix::GPointf3), srcbbb, (const float(*)[4])FujiM, dstbbb);

    /* ライブラリ用変換行列(ドット座標→正規化)生成 */
    float dispM[4][4];
    Matrix::LoadIdentity(dispM);
    Matrix::ScaleMatrix(1/(1280/2.f), 1/(720/2.f), 1, dispM);
    Matrix::TranslateMatrix(-1280,-720, 0, dispM);
//    Matrix::ScaleMatrix(1, -1, 1, dispM);

    /* ライブラリ用変換行列適用 */
//    memcpy(srcaaa, dstaaa, sizeof(srcaaa));
//    Matrix::ConvVertexf3(12, srcaaa, (const float(*)[4])dispM, dstaaa);
    memcpy(srcbbb, dstbbb, sizeof(srcbbb));
    Matrix::ConvVertexf3(sizeof(srcbbb)/sizeof(Matrix::GPointf3), srcbbb, (const float(*)[4])dispM, dstbbb);

    /* OpenGL描画 ここから */
	GLuint aPclPositionHandle = glGetAttribLocation(mProgram, "vPclPosition");
	GLenum errcode=glGetError();
	if(errcode!=GL_NO_ERROR) __android_log_assert("aaaaa", "aaaaa", "aaaaa");

//    glVertexAttribPointer(aPclPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, dstaaa);
    glVertexAttribPointer(aPclPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, dstbbb);
	glEnableVertexAttribArray(aPclPositionHandle);

	glUseProgram(mProgram);
	glLineWidth(10);
//    glDrawArrays(GL_LINES, 0, 12);
    glDrawArrays(GL_LINES, 0, sizeof(srcbbb)/sizeof(Matrix::GPointf3));
    /* OpenGL描画 ここまで */
}

/******************/
/* CppSurfaceView2 */
/******************/
CppSurfaceView2::CppSurfaceView2(int id) : mId(id), mStatus(CppSurfaceView2::STATUS_NONE), mThreadId(-1) {
	pFujiDmyPcl = new FujiDmyPcl();
}

void CppSurfaceView2::createThread(JNIEnv *pEnv, jobject surface) {
    mStatus = CppSurfaceView2::STATUS_INITIALIZING;
    mWindow = ANativeWindow_fromSurface(pEnv, surface);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&mThreadId, &attr, CppSurfaceView2::draw_thread, (void*)this);
}

void *CppSurfaceView2::draw_thread(void *pArg) {
    if(pArg == NULL) return NULL;
    CppSurfaceView2 *pSurface = (CppSurfaceView2*)pArg;

    pSurface->initEGL();
    pSurface->initGL();
    pSurface->mStatus = CppSurfaceView2::STATUS_DRAWING;

    pSurface->predrawGL();
    struct timespec req = {0, (int)(16.66*1000000)};
    for(;pSurface->mStatus==CppSurfaceView2::STATUS_DRAWING;) {

        /* SurfaceCreated()が動作した時は、画面サイズ変更を実行 */
        if(pSurface->isSurfaceCreated) {
            pSurface->isSurfaceCreated = false;
            glViewport(0,0,pSurface->DspW,pSurface->DspH);
            __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "glViewport(%d,%d)", pSurface->DspW, pSurface->DspH);
        }

        /* 通常の描画処理 */
        pSurface->drawGL();
        nanosleep(&req, NULL);
    }

    pSurface->finEGL();

    return NULL;
}

void CppSurfaceView2::initEGL() {
	EGLint major, minor;
	mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(mEGLDisplay, &major, &minor);

	/* 設定取得 */
	const EGLint configAttributes[] = {
		EGL_LEVEL, 0,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		/* 透過設定 */
		EGL_ALPHA_SIZE, EGL_OPENGL_BIT,
		/*EGL_BUFFER_SIZE, 32 */  /* ARGB8888用 */
		EGL_DEPTH_SIZE, 16,
		EGL_NONE
	};

	EGLConfig config;
	EGLint numConfigs;
	eglChooseConfig(mEGLDisplay, configAttributes, &config, 1, &numConfigs);

	/* context生成 */
	const EGLint contextAttributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	mEGLContext = eglCreateContext(mEGLDisplay, config, EGL_NO_CONTEXT, contextAttributes);

	/* ウィンドウバッファサイズとフォーマットを設定 */
	EGLint format;
	eglGetConfigAttrib(mEGLDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(mWindow, 0, 0, format);

	/* surface生成 */
	mEGLSurface = eglCreateWindowSurface(mEGLDisplay, config, mWindow, NULL);
	if(mEGLSurface == EGL_NO_SURFACE) {
		__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView2", "surface生成 失敗!!");
		return;
	}

	/* context再生成 */
	mEGLContext = eglCreateContext(mEGLDisplay, config, EGL_NO_CONTEXT, contextAttributes);

	/* 作成したsurface/contextを関連付け */
	if(eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface, mEGLContext) == EGL_FALSE) {
		__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView2", "surface/contextの関連付け 失敗!!");
		return;
	}

	/* チェック */
	EGLint w,h;
	eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_WIDTH, &w);
	eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_HEIGHT,&h);
	glViewport(0,0,w,h);
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "glViewport(%d,%d) 1", w,h);
}

void CppSurfaceView2::initGL() {
    mProgram = createProgram(0, VERTEXSHADER, FRAGMENTSHADER);
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa CppSurfaceView2::mProgram=%d", mProgram);
    mLineProgram = createProgram(1, LINEVSHADER, LINEFSHADER);
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa CppSurfaceView2::mLineProgram=%d", mLineProgram);

	pFujiDmyPcl->createProgram();
}

GLuint CppSurfaceView2::createProgram(int seqno, const char *vertexshader, const char *fragmentshader) {
    GLuint vhandle = loadShader(GL_VERTEX_SHADER, vertexshader);
    if(vhandle == GL_FALSE) return GL_FALSE;

    GLuint fhandle = loadShader(GL_FRAGMENT_SHADER, fragmentshader);
    if(fhandle == GL_FALSE) return GL_FALSE;

    GLuint programhandle = glCreateProgram();
    if(programhandle == GL_FALSE) {
        checkGlError("glCreateProgram");
        return GL_FALSE;
    }

    glAttachShader(programhandle, vhandle);
    checkGlError("glAttachShader(VERTEX_SHADER)");
    glAttachShader(programhandle, fhandle);
    checkGlError("glAttachShader(FRAGMENT_SHADER)");

    if(seqno == 0)
        glBindAttribLocation( programhandle, seqno, "vPosition");
    else if(seqno == 1)
        glBindAttribLocation( programhandle, seqno, "vLinePosition");

    glLinkProgram(programhandle);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(programhandle, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE) {
        GLint bufLen = 0;
        glGetProgramiv(programhandle, GL_INFO_LOG_LENGTH, &bufLen);
        if(bufLen) {
            char *logstr = (char*)malloc(bufLen);
            glGetProgramInfoLog(mProgram, bufLen, NULL, logstr);
            __android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView2", "%d glLinkProgram() Fail!!\n %s", mId, logstr);
            free(logstr);
        }
        glDeleteProgram(programhandle);
        programhandle = GL_FALSE;
    }

    /* 後処理 */
    glDeleteShader(vhandle);
    glDeleteShader(fhandle);

    return programhandle;
}

GLuint CppSurfaceView2::loadShader(int shadertype, const char *sourcestring) {
    GLuint shaderhandle = glCreateShader(shadertype);
    if(!shaderhandle) return GL_FALSE;

    glShaderSource(shaderhandle, 1, &sourcestring, NULL);
    glCompileShader(shaderhandle);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shaderhandle, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shaderhandle, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen) {
            char *logbuf = (char*)malloc(infoLen);
            if(logbuf) {
                glGetShaderInfoLog(shaderhandle, infoLen, NULL, logbuf);
                __android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView2", "%d shader failuer!!\n%s", mId, logbuf);
                free(logbuf);
            }
        }
        glDeleteShader(shaderhandle);
        shaderhandle = GL_FALSE;
    }

    return shaderhandle;
}

void CppSurfaceView2::checkGlError(const char *argstr) {
    for(GLuint error = glGetError(); error; error = glGetError())
        __android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView2", "%d after %s errcode=%d", mId, argstr, error);
}

void CppSurfaceView2::predrawGL() {
    GLuint a_PositionHandle = glGetAttribLocation(mProgram, "vPosition");
    mu_rotMatrixHandle = glGetUniformLocation(mProgram, "u_rotMatrix");
    static const GLfloat vertexes[] = {
       -0.5, 0.5, 0,
       -0.5,-0.5, 0,
        0.5, 0.5, 0,

        0.5, 0.5, 0,
       -0.5,-0.5, 0,
        0.5,-0.5, 0,
    };
    glVertexAttribPointer(a_PositionHandle, 3, GL_FLOAT, GL_FALSE, 0, vertexes);
    glEnableVertexAttribArray(a_PositionHandle);

    mxPos = 0;
    myPos = 0;
    glClearColor(0, 0, 0, 0);

	pFujiDmyPcl->preDraw();
}

void CppSurfaceView2::drawGL() {
	if(DspW == 0 || DspH == 0)
		__android_log_assert("aaaaa", "aaaaa", "aaaaa");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* line用設定 ここから */
    GLuint a_LinePositionHandle = glGetAttribLocation(mLineProgram, "vLinePosition");
    GLenum errcode=glGetError();
    if(errcode!=GL_NO_ERROR) __android_log_assert("aaaaa", "aaaaa", "aaaaa");
	static const Matrix::GPointf3 srcvertexes[] = {
		/* 上辺 */{   0,  0, 0},{1280,  0, 0},
		/* 右辺 */{1280,  0, 0},{1280,720, 0},
		/* 左辺 */{   0,  0, 0},{   0,720, 0},
		/* 下辺 */{   0,720, 0},{1280,720, 0},

		/* 上辺 */{320,180, 0},{960,180, 0},
		/* 右辺 */{960,180, 0},{960,540, 0},
		/* 左辺 */{320,180, 0},{320,540, 0},
		/* 下辺 */{320,540, 0},{960,540, 0},
	};
	static Matrix::GPointf3 dstvertexes[sizeof(srcvertexes)/sizeof(Matrix::GPointf3)] = {0};

	float dispM[4][4];
	Matrix::LoadIdentity(dispM);
	Matrix::ScaleMatrix(1/(1280/2.f), 1/(720/2.f), 1, dispM);
	Matrix::TranslateMatrix(-1,-1, 0, dispM);
	Matrix::ScaleMatrix(1, -1, 1, dispM);
	Matrix::ConvVertexf3(sizeof(srcvertexes)/sizeof(Matrix::GPointf3), srcvertexes, (const float(*)[4])dispM, dstvertexes);
    glVertexAttribPointer(a_LinePositionHandle, 3, GL_FLOAT, GL_FALSE, 0, dstvertexes);
    glEnableVertexAttribArray(a_LinePositionHandle);

    glUseProgram(mLineProgram);
    glLineWidth(10);
    glDrawArrays(GL_LINES, 0, 16);
	/* line用設定 ここまで */

	pFujiDmyPcl->draw();

	if(DspW != 0 && DspH != 0) {
        mxPos += mMoveX;
        myPos += mMoveY;
        if((mxPos > (2*DspW)) || (mxPos < 0)) mMoveX = -mMoveX;
        if((myPos > (2*DspH)) || (myPos < 0)) mMoveY = -mMoveY;
        float translateMatrix[4][4];
        Matrix::LoadIdentity(translateMatrix);
        Matrix::TranslateMatrix(mxPos/DspW-1, myPos/DspH-1, 0, translateMatrix);
//		Matrix::TranslateMatrix(DspW/2.f/DspW, DspH/2.f/DspH, 0, translateMatrix);	/* 1st ドット座標中心へ移動 */
//		Matrix::ScaleMatrix(1, -1, 1, translateMatrix);								/* 2nd Y軸反転 */
//		Matrix::ScaleMatrix(1.1, 1, 0, translateMatrix);							/* 3rd アスペクト比 */
//		Matrix::RotateMatrix(36,0,0,1,translateMatrix);								/* 4th 回転 */

		/* OpenGL用に転置 */
        float glM[] = {
            translateMatrix[0][0],translateMatrix[1][0],translateMatrix[2][0],translateMatrix[3][0],
            translateMatrix[0][1],translateMatrix[1][1],translateMatrix[2][1],translateMatrix[3][1],
            translateMatrix[0][2],translateMatrix[1][2],translateMatrix[2][2],translateMatrix[3][2],
            translateMatrix[0][3],translateMatrix[1][3],translateMatrix[2][3],translateMatrix[3][3]
        };
//        glUseProgram(mProgram);
//        glUniformMatrix4fv(mu_rotMatrixHandle, 1, false, glM);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

	eglSwapBuffers(mEGLDisplay, mEGLSurface);
}

void CppSurfaceView2::finEGL() {
    __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa%d CppSurfaceView2::finEGL()", mId);
    ANativeWindow_release(mWindow);
    mWindow = NULL;
}

void CppSurfaceView2::destroy() {
    __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa%d CppSurfaceView2::destroy()", mId);
    pthread_join(mThreadId, NULL);
}

CppSurfaceView2::~CppSurfaceView2() {
	delete pFujiDmyPcl;
	pFujiDmyPcl = nullptr;
}
