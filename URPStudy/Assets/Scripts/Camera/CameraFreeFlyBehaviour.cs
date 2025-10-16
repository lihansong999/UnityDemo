using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class CameraFreeFlyBehaviour : MonoBehaviour
{
    private float _moveSpeed = 10f;
    private float _verticalSpeed = 10f;

    private float _rotationX = 0f;
    private float _rotationY = 0f;

    void Start()
    {
        Vector3 angles = transform.eulerAngles;
        _rotationX = angles.x;
        _rotationY = angles.y;
    }

    void Update()
    {
        if (GameManager.instance.curGameStatus == GameStatus.Watching && !GameManager.instance.IsShowingUIForGameOperation())
        {
            HandleMouseRotation();
            HandleMovement();
        }
    }

    private void HandleMouseRotation()
    {
        float mouseX = Input.GetAxis("Mouse X") * GlobalValueManager.instance.mouseSensitivity * Time.deltaTime;
        float mouseY = Input.GetAxis("Mouse Y") * GlobalValueManager.instance.mouseSensitivity * Time.deltaTime;

        _rotationY += mouseX;
        _rotationX -= mouseY;
        _rotationX = Mathf.Clamp(_rotationX, -90, 90);

        transform.rotation = Quaternion.Euler(_rotationX, _rotationY, 0f);
    }

    private void HandleMovement()
    {
        float speed = Input.GetKey(KeyCode.LeftShift) ? _moveSpeed * 2f : _moveSpeed;
        Vector3 move = Vector3.zero;
        move += transform.forward * Input.GetAxis("Vertical") * speed;
        move += transform.right * Input.GetAxis("Horizontal") * speed;

        if (Input.GetKey(KeyCode.E)) 
            move += Vector3.up * _verticalSpeed;
        if (Input.GetKey(KeyCode.Q)) 
            move -= Vector3.up * _verticalSpeed;

        transform.position += move * Time.deltaTime;
    }

    // 瞬间切换到某个目标上方向下看着它
    public void ToTargetUp(GameObject toTarget)
    {
        //transform.localPosition = toTarget.transform.localPosition + Vector3.up * 10;
        transform.position = toTarget.transform.position + Vector3.up * 10f;
        //transform.LookAt(toTarget.transform.position);
        _rotationX = 90f;
        _rotationY = 0f;
        transform.rotation = Quaternion.Euler(_rotationX, _rotationY, 0f);
    }
}
