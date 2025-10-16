using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NetSpinnerBehaviour : MonoBehaviour
{
    [SerializeField] private float rotateSpeed = 100f;

    void Update()
    {
        transform.Rotate(0, 0, -rotateSpeed * Time.deltaTime);
    }
}
