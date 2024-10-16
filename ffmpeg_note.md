
### 文件结构体 AVHWDeviceContext

* 申请方式：`av_hwdevice_ctx_alloc` 申请，参数传入`AVHWDeviceType`类型即可，返回`AVBufferRef`
* 结构体中包含`AVHWDeviceInternal`，该结构体含有`HWDeviceType`，存在很多对结构体操作的函数指针，用于进行硬件操作。
* 
