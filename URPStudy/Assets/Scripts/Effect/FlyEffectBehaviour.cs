using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FlyEffectBehaviour : EntityBehaviour
{
    protected long _launchTime = 0;
    protected Vector3 _launchPosition = new Vector3();
    protected Vector3 _flyVelocity = new Vector3();

    protected long _completeTime = 0;
    protected Vector3 _completePosition = new Vector3();

    protected bool _boom = false;

    protected virtual void Start()
    {

    }

    protected virtual void Update()
    {
        long st = TimeSyncManager.instance.GetServerTime();
        float dt = (st - _launchTime) / 1000f;
        transform.position = _launchPosition + _flyVelocity * dt;
        if (_completeTime != 0 && _completeTime - st < 16)
        {
            if (_boom)
            {
                // 播放技能命中爆炸特效
                GameManager.instance.PlayMagicBoom(_launchPosition + _flyVelocity * ((_completeTime - _launchTime) / 1000f));
            }
            GameManager.instance.DestroyMagic(_entityID);
        }
    }
}
