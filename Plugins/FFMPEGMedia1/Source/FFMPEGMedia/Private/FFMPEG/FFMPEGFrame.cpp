#include "FFMPEGFrame.h"

FFMPEGFrame::FFMPEGFrame()
{
    frame = NULL;
    serial = 0;
    pts = 0.0;           
    duration = 0.0;      
    pos = 0;          
    width = 0;
    height = 0;
    format = 0;    
    uploaded = false;
    flip_v = false;
    sub = {0};
}

FFMPEGFrame::~FFMPEGFrame()
{
    Destroy();
}

int FFMPEGFrame::Init () {
    Destroy();
    frame = av_frame_alloc();
    return frame == 0? 0: 1;
}

void FFMPEGFrame::Destroy() {
    if (frame != NULL) {
        UnRef();
        av_frame_free(&frame);
    }
    frame = NULL;
}

void FFMPEGFrame::UnRef() {
    if ( frame != NULL) {
        av_frame_unref(frame);
        avsubtitle_free(&sub);
    }
}

int FFMPEGFrame::GetSerial() {
    return serial;
}

int64_t FFMPEGFrame::GetPos() {
    return pos;
}

double FFMPEGFrame::GetPts() {
    return pts;
}

double FFMPEGFrame::GetDuration() {
    return duration;
}

AVFrame* FFMPEGFrame::GetFrame() {
    return frame;
}

int FFMPEGFrame::GetWidth() {
    return width;
}

int FFMPEGFrame::GetHeight() {
    return height;
}

int FFMPEGFrame::GetFormat() {
    return format;
}

AVRational FFMPEGFrame::GetSar() {
    return sar;
}

bool FFMPEGFrame::IsUploaded() {
    return uploaded;
}

bool FFMPEGFrame::IsVerticalFlip() {
    return flip_v;
}

AVSubtitle& FFMPEGFrame::GetSub() {
    return sub;
}

double FFMPEGFrame::GetDifference(FFMPEGFrame* nextvp, double max) {

    if (serial == nextvp->serial) {
        double v_duration = nextvp->pts - pts;
        if (isnan(v_duration) || v_duration <= 0 || v_duration > max)
            return GetDuration();
        else
            return v_duration;
    }
    else {
        return 0.0;
    }
}

void FFMPEGFrame::UpdateFrame(AVFrame* src_frame, double v_pts, double v_duration, int64_t v_pos, int v_serial) {
    this->sar = src_frame->sample_aspect_ratio;
    this->uploaded = 0;
    
    this->width = src_frame->width;
    this->height = src_frame->height;
    this->format = src_frame->format;
    
    this->pts = v_pts;
    this->duration = v_duration;
    this->pos = v_pos;
    this->serial = v_serial;
}


void FFMPEGFrame::UpdateSize(FFMPEGFrame *vp) {
    width = vp->width;
    height = vp->height;
}

void FFMPEGFrame::SetPts(double p) {
    this->pts = p;
}

void FFMPEGFrame::SetSerial(int s) {
    this->serial = s;
}

void FFMPEGFrame::SetWidth(int w) {
    this->width = w;
}

void FFMPEGFrame::SetHeight(int h) {
    this->height = h;
}

void FFMPEGFrame::SetUploaded(bool u) {
    this->uploaded = u;
}

void FFMPEGFrame::SetVerticalFlip(bool fv) {
    this->flip_v = fv;
}

void FFMPEGFrame::SetPos(int64_t p) {
    this->pos = p;
}

void FFMPEGFrame::SetDuration(double d) {
    this->duration = d;
}

