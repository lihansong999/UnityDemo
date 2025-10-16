using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.TextCore;

public class NetMagicFlyEffectBehaviour : FlyEffectBehaviour
{
    // Start is called before the first frame update
    protected override void Start()
    {
        base.Start();
    }

    // Update is called once per frame
    protected override void Update()
    {
        base.Update();
    }

    private void LaunchFrameHandle(in MsgSCFlyEffectFrame frame)
    {
        _launchTime = frame.frameInfo.curTime;
        _launchPosition.Set(NetWorkUtils.i2f(frame.frameInfo.positionX), NetWorkUtils.i2f(frame.frameInfo.positionY), NetWorkUtils.i2f(frame.frameInfo.positionZ));
        _flyVelocity.Set(NetWorkUtils.i2f(frame.frameInfo.velocityX), NetWorkUtils.i2f(frame.frameInfo.velocityY), NetWorkUtils.i2f(frame.frameInfo.velocityZ));
    }

    private void HitFrameHandle(in MsgSCFlyEffectFrame frame)
    {
        _completeTime = frame.frameInfo.curTime;
        _completePosition.Set(NetWorkUtils.i2f(frame.frameInfo.positionX), NetWorkUtils.i2f(frame.frameInfo.positionY), NetWorkUtils.i2f(frame.frameInfo.positionZ));
        _boom = true;
    }

    private void OutRangeFrameHandle(in MsgSCFlyEffectFrame frame)
    {
        _completeTime = frame.frameInfo.curTime;
        _completePosition.Set(NetWorkUtils.i2f(frame.frameInfo.positionX), NetWorkUtils.i2f(frame.frameInfo.positionY), NetWorkUtils.i2f(frame.frameInfo.positionZ));
    }

    public void AddNetFrame(in MsgSCFlyEffectFrame frame)
    {
        switch(frame.frameInfo.infoType)
        {
            case 1: // 发射
                LaunchFrameHandle(frame);
                break;
            case 2: // 命中
                HitFrameHandle(frame);
                break;
            case 3: // 消失（超出射程）
                OutRangeFrameHandle(frame);
                break;
        }
    }
}
