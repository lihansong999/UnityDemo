using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MagicBoomEffectBehaviour : MonoBehaviour
{
    public ParticleSystem _particleSystem;

    // Start is called before the first frame update
    void Start()
    {
        _particleSystem = GetComponent<ParticleSystem>();
        _particleSystem.Play();

        StartCoroutine(WaitForCompletion());
    }

    IEnumerator WaitForCompletion()
    {
        do
        {
            yield return null; // µÈ´ýÒ»Ö¡
        } while (_particleSystem.IsAlive());
        Destroy(gameObject);
    }
}
