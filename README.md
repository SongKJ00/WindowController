안드로이드 App으로 사용자가 설정한 시간에 자동으로 창문이 열리고 닫히는 프로젝트입니다.

## 동작 시퀀스
1. 안드로이드 앱으로 window라는 블루투스와 연결한다.
2. 연결 후 원하는 시간을 세팅하고 설정 버튼을 눌러 시간 데이터를 전송한다.
3. LCD에는 설정한 시간과 현재 시간이 출력된다.
4. 현재 시간과 설정한 시간이 같을 경우 창문이 열린다.

## 사용 부품
* 현재 시간 획득 - DS3231(RTC)
* 설정한 시간 저장 - AT24(EEPROM)
* 블루투스 통신 - HC-06
* 현재 시간 및 설정 시간 표시 - LCD

---------------------------------------------------------------------------------------
