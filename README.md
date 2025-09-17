# Aventurine Vulnerable Drivers
- Vulnerable Driver Example
  - 이 저장소는 **보안 연구 및 교육 목적**으로 제작된 **취약한 Windows 커널 드라이버** 모음입니다. 각 드라이버는 특정 유형의 커널 취약점을 포함하고 있어, 퍼징, 익스플로잇 개발, 커널 디버깅 학습에 적합합니다.

> ⚠️ **경고**: 이 드라이버들은 실제 취약점을 포함하고 있습니다. 프로덕션 시스템에서는 절대 사용하지 마세요!

## 📋 목차

- [프로젝트 개요](#프로젝트-개요)
- [드라이버 목록](#드라이버-목록)
- [사용 방법](#사용-방법)
- [빌드 환경](#빌드-환경)
- [보안 경고](#보안-경고)
- [라이선스](#라이선스)

## 🎯 프로젝트 개요

이 프로젝트는 다음과 같은 목적을 위해 제작되었습니다:

- 커널 퍼징 테스트 대상 제공
- 커널 익스플로잇 개발 학습
- 커널 디버깅 기술 연습
- 다양한 취약점 유형 연구

## 🚩 드라이버 목록

### 1. BufferOverflowDriver.sys
**취약점 유형**: 버퍼 오버플로우
- 입력 길이 검증 없이 memcpy 수행
- 작은 내부 버퍼로 인한 스택 오버플로우

### 2. NullPtrDriver.sys
**취약점 유형**: 널 포인터 역참조
- 포인터 검증 없이 메모리 접근
- 사용자 입력으로부터 널 포인터 역참조 유도

### 3. IntegerOverflowDriver.sys
**취약점 유형**: 정수 오버플로우
- 곱셈 연산에서 오버플로우 발생
- 잘못된 메모리 할당으로 인한 문제

### 4. RaceConditionDriver.sys
**취약점 유형**: 경쟁 조건
- 동기화 없이 공유 데이터 접근
- 멀티스레드 환경에서의 데이터 손상

### 5. PrivilegeDriver.sys
**취약점 유형**: 권한 검사 누락
- 관리자 권한 없이 민감한 작업 수행
- 권한 상승 테스트용

### 6. FormatStringDriver.sys
**취약점 유형**: 포맷 스트링
- DbgPrint에 포맷 스트링 직접 전달
- 포맷 스트링 공격 테스트

### 7. FileSystemDriver.sys
**취약점 유형**: IRP 처리 취약점
- IRP_MJ_CREATE/READ/WRITE 처리 문제
- 정보 누출 및 버퍼 오버플로우

### 8. ProcessCallbackDriver.sys
**취약점 유형**: 콜백 등록 취약점
- ObRegisterCallbacks 관련 문제
- 메모리 릭 및 배열 오버플로우

### 9. TimerDriver.sys
**취약점 유형**: 타이머 DPC 취약점
- 반복적 메모리 할당 릭
- 타이머 관리 오류

### 10. NetworkDriver.sys
**취약점 유형**: 네트워크 패킷 처리
- 패킷 버퍼 관리 문제
- 포맷 스트링 및 오버플로우

### 11. MemoryManagerDriver.sys
**취약점 유형**: 메모리 관리 오류
- 더블 프리, 배열 오버플로우
- 메모리 릭 및 해제 오류

## 🔧 사용 방법

### 드라이버 설치
```cmd
# 관리자 권한으로 실행
sc create VulnerableDriver binPath= "C:\path\to\driver.sys" type= kernel
sc start VulnerableDriver
```
### 드라이버 제거
```
sc stop VulnerableDriver
sc delete VulnerableDriver
```
