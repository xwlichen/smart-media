package com.smart.demo.seekbar;

import android.animation.ValueAnimator;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Shader;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ViewGroup;


import com.smart.demo.R;

import java.math.BigDecimal;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;


/**
 * @date : 2018/12/11 下午3:52
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public class MusicSeekBar extends ViewGroup {


    private int colorFTrack; //进度条颜色
    private int colorSTrack;//缓冲条颜色
    private int colorTTrack; //当前进度条颜色
    private Paint paintFTrack;
    private Paint paintSTrack;
    private Paint paintTTrack;


    private int thum;
    private int thumbWidth;
    private int thumbHeight;
    private float thumbLeft;
    private float thumbTop;
    private Bitmap thumbBitmap;
    private Paint thumPaint;
    private int thumColor;

    Rect srcThum = null;
    Rect dstThum = null;


    private int trackHeight;
    private float trackWidth;

    private float currentProgress = 0;
    private float lastProgress;
    private float max;
    private float min;
//    private int model; //0-中心位置作为移动标准 1-左边位置占座位移动标准


    private int width;
    private int height;

    private int paddingLeft, paddingRigtht, paddingTop, paddingBottom;
    private float faultTolerance = -1;//the tolerance for user seek bar touching
    private int decimalScale = 1;//保留小数点几位

    private float secondWidth;

    private boolean isTouching;
    private SeekChangeListener seekChangeListener;

    private Context context;

    private ValueAnimator valueAnimator;
    private boolean canUpdate = true;
    private int maxScaleValue = 6; //最大的缩放值
    private float loadingValue;//loading的时候缩放的变化值
    private boolean isLoading = false;//是否正在加载


    public MusicSeekBar(Context context) {
        super(context);
        this.context = context;
        init();
    }

    public MusicSeekBar(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        this.context = context;
        initAttrs(attrs);
        init();
    }

    public MusicSeekBar(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        this.context = context;
        initAttrs(attrs);
        init();
    }

    private void initAttrs(AttributeSet attrs) {
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.MusicSeekBar);
        //dp
        trackHeight = (int) typedArray.getDimension(R.styleable.MusicSeekBar_trackSize, 2 * 3);

        colorFTrack = typedArray.getColor(R.styleable.MusicSeekBar_trackFColor, Color.parseColor("#E0E0E0"));
        colorSTrack = typedArray.getColor(R.styleable.MusicSeekBar_trackSColor, Color.parseColor("#8EADCD"));
        colorTTrack = typedArray.getColor(R.styleable.MusicSeekBar_trackTColor, Color.parseColor("#FF8820"));

        thum = typedArray.getResourceId(R.styleable.MusicSeekBar_thum, 0);
        thumColor = typedArray.getColor(R.styleable.MusicSeekBar_thumColor, Color.BLACK);

        thumbWidth = (int) typedArray.getDimension(R.styleable.MusicSeekBar_thumWidth, 0);
        thumbHeight = (int) typedArray.getDimension(R.styleable.MusicSeekBar_thumHeight, 0);

        max = typedArray.getInt(R.styleable.MusicSeekBar_max, 0);
        min = typedArray.getInt(R.styleable.MusicSeekBar_min, 0);

        loadingValue = maxScaleValue;
    }


    public void init() {
        setWillNotDraw(false);
        thumbBitmap = BitmapFactory.decodeResource(getResources(), thum);


        if (thumbWidth == 0) {
            thumbWidth = thumbBitmap.getWidth();
        }
        if (thumbHeight == 0) {
            thumbHeight = thumbBitmap.getHeight();
        }


        initPaint();


    }


    public void initPaint() {
        paintFTrack = new Paint();
        paintFTrack.setAntiAlias(true);
        paintFTrack.setStrokeCap(Paint.Cap.ROUND);
        paintFTrack.setStrokeWidth(trackHeight);
        paintFTrack.setColor(colorFTrack);

        paintSTrack = new Paint();
        paintSTrack.setAntiAlias(true);
        paintSTrack.setStrokeCap(Paint.Cap.ROUND);
        paintSTrack.setStrokeWidth(trackHeight);
        paintSTrack.setColor(colorSTrack);


        paintTTrack = new Paint();
        paintTTrack.setAntiAlias(true);
        paintTTrack.setStrokeCap(Paint.Cap.ROUND);
        paintTTrack.setStrokeWidth(trackHeight);
        paintTTrack.setColor(colorTTrack);


        thumPaint = new Paint();
        thumPaint.setAntiAlias(true);
        thumPaint.setStrokeCap(Paint.Cap.ROUND);
        thumPaint.setColor(thumColor);


    }

    /**
     * 根据Model返回值
     *
     * @param value
     * @return
     */
    private int measure(int value) {
        int result = 0;
        int specMode = MeasureSpec.getMode(value);
        int specSize = MeasureSpec.getSize(value);
        switch (specMode) {
            case MeasureSpec.EXACTLY:
            case MeasureSpec.AT_MOST:
                result = specSize;
                break;
            default:
                break;
        }

        return result;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        width = measure(widthMeasureSpec);
        height = measure(heightMeasureSpec);
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        if (thumbBitmap == null) {
            setPadding(maxScaleValue, maxScaleValue, maxScaleValue, maxScaleValue);
        }


        paddingLeft = getPaddingLeft() + thumbWidth / 2;
        paddingTop = getPaddingTop();
        paddingRigtht = getPaddingRight() + thumbWidth / 2;
        paddingBottom = getPaddingBottom();
        trackWidth = width - paddingLeft - paddingRigtht;
        trackLeft = paddingLeft;


        trackTop = (height) / 2;
        trackRight = paddingRigtht;
        trackBottom = trackTop;

        thumbTop = (height - thumbHeight) / 2;


    }


    @Override
    protected void onLayout(boolean b, int i, int i1, int i2, int i3) {


    }


    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (width <= 0) {
            setVisibility(GONE);
            return;
        } else {
            setVisibility(VISIBLE);
        }

        drawFirstTrack(canvas);
        drawSecondTrack(canvas);
        drawThirdTrack(canvas);


    }


    float trackLeft, trackTop, trackRight, trackBottom;

    public void drawFirstTrack(Canvas canvas) {
        canvas.drawLine(trackLeft + trackHeight, trackTop, trackLeft + trackWidth - trackHeight, trackBottom, paintFTrack);
    }

    public void drawSecondTrack(Canvas canvas) {
        float secondValue;
        if ((trackLeft + secondWidth - trackHeight) < (trackLeft + trackHeight)) {
            secondValue = trackLeft + trackHeight;
        } else {
            secondValue = trackLeft + secondWidth - trackHeight;
        }
        canvas.drawLine(trackLeft + trackHeight, trackTop, secondValue, trackBottom, paintSTrack);
    }

    public void drawThirdTrack(Canvas canvas) {
        //两个坐标形成变量，规定了渐变的方向和间距大小，着色器为镜像
        LinearGradient linearGradient = new LinearGradient(trackLeft, trackTop, trackLeft + trackWidth, trackBottom, Color.parseColor("#FF8647"), Color.parseColor("#FF5E45"), Shader.TileMode.MIRROR);
//        paintTTrack.setShader(linearGradient);
        //取消渐变色，换成同一颜色
        paintTTrack.setColor(colorTTrack);


        if (thumbLeft < (trackLeft)) {
            canvas.drawLine(trackLeft + trackHeight, trackTop, trackLeft + trackHeight, trackBottom, paintTTrack);
        } else {
            canvas.drawLine(trackLeft + trackHeight, trackTop, thumbLeft + trackLeft + 1, trackBottom, paintTTrack);
        }

        if (thumbBitmap != null) {
            srcThum = new Rect(0, 0, thumbBitmap.getWidth(), thumbBitmap.getHeight());
            dstThum = new Rect((int) (thumbLeft), (int) (thumbTop), (int) (thumbLeft + thumbWidth), (int) (thumbTop + thumbHeight));

            canvas.drawBitmap(thumbBitmap, srcThum, dstThum, null);
        } else {
            if (isTouching) {
                canvas.drawCircle(thumbLeft + thumbWidth / 2f + maxScaleValue, height / 2, thumbWidth / 2f + maxScaleValue, thumPaint);
            } else {
                canvas.drawCircle(thumbLeft + thumbWidth / 2f + maxScaleValue, height / 2, thumbWidth / 2f + maxScaleValue - loadingValue, thumPaint);
            }


        }


    }


    boolean mOnlyThumbDraggable;//only drag the seek bar's thumb can be change the progress

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (!isEnabled()) {
            return false;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                performClick();
                float mX = event.getX();
                if (isTouchSeekBar(mX, event.getY())) {
                    if ((mOnlyThumbDraggable && !isTouchThumb(mX))) {
                        return false;
                    }
                    isTouching = true;
                    if (seekChangeListener != null) {
                        seekChangeListener.onStartTrackingTouch(this);
                    }
                    refreshSeekBar(event);
                    return true;
                }
                break;
            case MotionEvent.ACTION_MOVE:
                refreshSeekBar(event);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                isTouching = false;
                if (seekChangeListener != null) {
                    seekChangeListener.onStopTrackingTouch(this);
                }
//                if (!autoAdjustThumb()) {
                showLoading();
                invalidate();

//                }

                break;
            default:
                break;
        }
        return super.onTouchEvent(event);
    }


    private boolean isTouchThumb(float mX) {
        float rawTouchX;
        refreshThumbCenterXByProgress(currentProgress);

        rawTouchX = thumbLeft;

        return rawTouchX - thumbWidth / 2f <= mX && mX <= rawTouchX + thumbWidth / 2f;
    }

    private void refreshThumbCenterXByProgress(float progress) {

        thumbLeft = (int) ((progress / max) * ((trackWidth) * 1.0f));


    }


    private boolean isTouchSeekBar(float mX, float mY) {
        if (faultTolerance == -1) {
            //dp
            faultTolerance = 5 * 3;
        }
        boolean inWidthRange = mX >= (paddingLeft - 2 * faultTolerance) && mX <= (width - paddingRigtht + 2 * faultTolerance);
        boolean inHeightRange = mY >= (trackTop - thumbHeight / 2 - faultTolerance) && mY <= (trackTop + thumbHeight / 2 + faultTolerance);
        return inWidthRange && inHeightRange;
    }


    public void setOnSeekChangeListener(@NonNull SeekChangeListener listener) {
        this.seekChangeListener = listener;
    }

    private void setSeekListener(boolean formUser) {
        if (seekChangeListener == null) {
            return;
        }
        if (progressChange()) {
            seekChangeListener.onSeeking(collectParams(formUser));
        }
    }

    private void refreshSeekBar(MotionEvent event) {
        refreshThumbCenterXByProgress(calculateProgress(calculateTouchX(adjustTouchX(event))));
        setSeekListener(true);
        requestLayout();
        showLoading();
        invalidate();
//        updateIndicator();
    }

    public void showLoading() {
        if (!isLoading) {
            return;
        }

        if (valueAnimator == null) {
            valueAnimator = ValueAnimator.ofFloat(0f, maxScaleValue, 0f);
            valueAnimator.setDuration(1500);
            valueAnimator.setRepeatCount(ValueAnimator.INFINITE);
            valueAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator valueAnimator) {
                    loadingValue = (float) valueAnimator.getAnimatedValue();
                    invalidate();

                }
            });
        }
        if (valueAnimator.isRunning()) {
            valueAnimator.cancel();
        }


        valueAnimator.start();


    }

    public void hideLoading() {
        if (valueAnimator != null) {
            valueAnimator.cancel();

        }
    }

    private float adjustTouchX(MotionEvent event) {
        float touchXCache;
        if (event.getX() < paddingLeft) {
            touchXCache = 0;
        } else if (event.getX() > trackWidth) {
            touchXCache = trackWidth;
        } else {
            touchXCache = event.getX();
        }
        return touchXCache;
    }

    private float calculateProgress(float touchX) {
        lastProgress = currentProgress;
        currentProgress = (min + (getAmplitude())) * (touchX) / (trackWidth * 1.0f);
//        Log.e("xw", "seeke currentProgress:" + currentProgress);
        return currentProgress;
    }


    private float calculateTouchX(float touchX) {
        float touchXTemp = touchX;
        //make sure the seek bar to seek smoothly always
        // while the tick's count is less than 3(tick's count is 1 or 2.).

        return touchXTemp;
    }

    private float getAmplitude() {
        return (max - min) > 0 ? (max - min) : 1;
    }


    private boolean progressChange() {
        return lastProgress != currentProgress;

    }

    SeekParams seekParams;

    private SeekParams collectParams(boolean formUser) {
        if (seekParams == null) {
            seekParams = new SeekParams(this);
        }
        seekParams.progress = getProgress();
        seekParams.progressFloat = getProgressFloat();
        seekParams.fromUser = formUser;
        //for discrete series seek bar

        return seekParams;
    }

    public int getProgress() {
        return Math.round(currentProgress);
    }

    public synchronized float getProgressFloat() {
        BigDecimal bigDecimal = BigDecimal.valueOf(currentProgress);
        return bigDecimal.setScale(decimalScale, BigDecimal.ROUND_HALF_UP).floatValue();
    }


    public float getMax() {
        return max;
    }

    public void setMax(float max) {
        this.max = max;
    }

    public float getMin() {
        return min;
    }

    public void setMin(float min) {
        this.min = min;
    }

    public float getCurrentProgress() {
        return currentProgress;
    }

    public void setCurrentProgress(float currentProgress) {
        if (canUpdate) {
            this.currentProgress = currentProgress;
            refreshThumbCenterXByProgress(currentProgress);
            invalidate();
        }
    }


    public void setSecondProgress(int percent) {
        secondWidth = ((trackWidth) * (percent / 100.0f));
        invalidate();
    }

    public int getCurrentPercent() {
        int percent = (int) (currentProgress / max);
        Log.e("xw", "currentPercent:" + percent);
        return percent;
    }

    public void setCanUpdate(boolean canUpdate) {
        this.canUpdate = canUpdate;
    }

    public boolean isLoading() {
        return isLoading;
    }

    public void setLoading(boolean loading) {
        isLoading = loading;
    }
}
