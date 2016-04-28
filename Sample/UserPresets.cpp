#include "StdAfx.h"
#include "UserPresets.h"


static const lc_transcode_preset_t s_preset[] = {
    {
        "1280x720,30fps,1.5Mbps", /*presetName*/
        "高清,720p,16:9", /*description*/
        false, /* forward_only */
        {128000, 44100, 2, true}, /*audio*/
        {H264, 1400000, 3000, 1280, 720, KEEP, {HIGH}, true}, /*video*/
        {2, 3, 0, true}, /*hls*/
        {false, true}, /*rtmp*/
    },
    {
        "960x720,30fps,1.2Mbps",
        "高清,720p,4:3",
        false, /* forward_only */
        {128000, 44100, 2, true},
        {H264, 1100000, 3000, 960, 720, KEEP, {HIGH}, true},
        {2, 3, 0, true}, /*hls*/
        {false, true}, /*rtmp*/

    },
    {
        "854x480,25fps,800kbps",
        "高清,480p,16:9",
        false, /* forward_only */
        {80000, 22050, 2, true},
        {H264, 720000, 2500, 854, 480, KEEP, {HIGH}, true},
        {2, 3, 0, true}, /*hls*/
        {false, true}, /*rtmp*/
    },
    {
        "640x480,25fps,620kbps",
        "高清,480p,4:3",
        false, /* forward_only */
        {80000, 22050, 2, true},
        {H264, 540000, 2500, 640, 480, KEEP, {HIGH}, true},
        {2, 3, 0, true}, /*hls*/
        {false, true}, /*rtmp*/
    },
    {
        "640x360,24fps,520kbps",
        "标清,360p,16:9",
        false, /* forward_only */
        {80000, 22050, 2, true},
        {H264, 440000, 2400, 640, 360, KEEP, {MAIN}, true},
        {2, 3, 0, true}, /*hls*/
        {false, true}, /*rtmp*/
    },
    {
        "480x360,24fps,440kbps",
        "标清,360p,4:3",
        false, /* forward_only */
        {80000, 22050, 2, true},
        {H264, 360000, 2400, 480, 360, KEEP, {MAIN}, true},
        {2, 3, 0, true}, /*hls*/
        {false, true}, /*rtmp*/
    },
    {
        "320x240,20fps,240kbps",
        "低清,240p,4:3",
        false, /* forward_only */
        {40000, 22050, 2, true},
        {H264, 200000, 2000, 320, 240, KEEP, {BASELINE}, true},
        {2, 3, 0, true}, /*hls*/
        {false, true}, /*rtmp*/
    },
    {
        NULL
    }
};


const lc_transcode_preset_t* g_GetUserPresets() {
    return &s_preset[0];
}