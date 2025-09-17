import ctypes
import random
import numpy as np
from keras.models import Sequential
from keras.layers import Dense
from keras.optimizers import Adam

# Windows API 호출
kernel32 = ctypes.windll.kernel32
INVALID_HANDLE_VALUE = -1

# 드라이버와 연결
def open_driver():
    handle = kernel32.CreateFileW(
        "\\\\.\\MyDriver",
        0xC0000000,  # GENERIC_READ | GENERIC_WRITE
        0,
        None,
        3,  # OPEN_EXISTING
        0,
        None
    )
    if handle == INVALID_HANDLE_VALUE:
        raise Exception("Failed to open driver")
    return handle

# 랜덤 입력 생성
def generate_random_input(size=512):
    return bytes(random.choices(range(256), k=size))

# AI 모델 정의
def create_model(input_size=512):
    model = Sequential([
        Dense(128, activation='relu', input_shape=(input_size,)),
        Dense(64, activation='relu'),
        Dense(input_size, activation='sigmoid')
    ])
    model.compile(optimizer=Adam(0.001), loss='binary_crossentropy')
    return model

# AI 기반 입력 생성
def generate_ai_input(model, input_size=512):
    noise = np.random.rand(1, input_size)
    output = model.predict(noise)[0]
    return bytes((output * 255).astype(int))

# IOCTL 호출
def send_ioctl(handle, data):
    bytes_returned = ctypes.c_ulong(0)
    result = kernel32.DeviceIoControl(
        handle,
        0x228000,  # IOCTL_MY_FUZZ
        data, len(data),
        None, 0,
        ctypes.byref(bytes_returned),
        None
    )
    return result

# 메인 퍼저 루프
def fuzz_loop():
    handle = open_driver()
    model = create_model()
    crash_inputs = []

    try:
        for i in range(1000):
            print(f"[+] Iteration {i}")

            # AI 입력 생성
            data = generate_ai_input(model)

            try:
                result = send_ioctl(handle, data)
                if not result:
                    print("[!] Possible crash detected")
                    crash_inputs.append(data)
            except Exception as e:
                print(f"[!] Crash: {e}")
                crash_inputs.append(data)

            # 피드백: 크래시 입력을 학습 데이터로 사용
            if len(crash_inputs) > 0:
                X = np.array([list(inp.ljust(512, b'\x00')[:512]) for inp in crash_inputs]) / 255.0
                model.fit(X, X, epochs=5, verbose=0)

    finally:
        kernel32.CloseHandle(handle)
        # 크래시 입력 저장
        for i, inp in enumerate(crash_inputs):
            with open(f"crash_input_{i}.bin", "wb") as f:
                f.write(inp)

if __name__ == "__main__":
    fuzz_loop()
