<p align="middle" >

 <img width="200px;" src = "https://user-images.githubusercontent.com/106827197/209891837-2d6620d6-cf61-4ce3-b7e7-6ba2422765be.png"/>

</p>
<h1 align="middle">TPS Shooting 멀티플레이 게임</h1>



## 🔍 진행방식

- 미션은 **기능 요구사항, 프로그래밍 요구사항, 과제 진행 요구사항** 세 가지로 구성되어 있다.
- 세 개의 요구사항을 만족하기 위해 노력한다. 특히 기능을 구현하기 전에 기능 목록을 만들고, 기능 단위로 커밋 하는 방식으로 진행한다.

---
## 🎯 기능 요구사항
TPS장르의 멀티플레이가 가능한 팀 vs 팀 FPS 게임을 구현한다

- 플레이어는 게임 접속시 회원가입 -> 로그인을 진행한다
- 로그인 이후 플레이어는 Host로 세션을 생성할지 Client로 세션에 참가할지 선택한다
- 이후 세션을 생성, 참가하면 로비 Level로 넘어가서 게임에 참여한 플레이어를 확인할 수 있다.
- 로비에서는 채팅을 통해 서로 소통할 수 있다
- 이후 로비에 모든 플레이어가 모이면 Host는 server travel을 통해 모든 플레이어를 인게임 Level로 이동시킨다.
- 인게임 Level에서는 A, B팀을 선택할 수 있고 일정시간이 지날때까지는(모든 플레이어가 접속할 때 까지는) 움직일 수 없다
- 이후 시간이 지나고 게임이 시작되면 A, B팀은 Level에 배치되어있는 무기를 파밍하고 전투를 시작한다.
- 마지막으로 게임이 끝난 이후에는 승리팀과 패배팀의 인터페이스가 각각 나타나고 로비로 이동해 동일 팀으로 진행할지, 아니면 다시 세션방으로 진입할지 결정한다.

<br>

### 💻 실행 결과

<img width="1259" alt="Untitled (2)" src="https://user-images.githubusercontent.com/106827197/209893043-d182ab50-af11-45b7-99ea-76b472d0eafc.png">

<br>

---
## ✅ 프로그래밍 요구사항
- 프로젝트 인원들은 프로젝트 폴더 안에서 각자의 파일에서만 작업을 허용한다, 또한 공유 자원을 필요로 한다면 서로 소통을 통해 컨펌 이후 작업을 진행한다. 
- 다음과 같이 c++ 클래스를 만들고, 인스턴스를 만들어 사용한다.
```c++
UCLASS()
class YGL_MULTIPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

```
### 라이브러리
- **UE4 에디터와 사설 소켓 서버를 연동해, 통신하기 위한 라이브러리는 [`CodeSpartan` 라이브러리](https://github.com/CodeSpartan/UE4TcpSocketPlugin.git)의 `TCP Socket Connection`을 사용해 구한다.
- **엔진 내에서 멀티플레이 네트워크 환경을 구축하기위한 세션 라이브러리는 [`mordentral` 라이브러리](https://github.com/mordentral/AdvancedSessionsPlugin.git)의 `AdvancedSession`을 사용해 구한다.
  

### 공통 요구사항

- **프로젝트 작업시 코드 최적화와 속도를 위해 가능한 c++ 클래스로 작업을 진행한다.
- **AI, UMG, InterFace작업시 필요 이상으로 복잡해지는 c++클래스 대신 UMG블루프린트를 사용해 작업한다.
- **클래스 작업시 공통 기능을 공유하는 클래스들은 [객체지향](https://docs.unrealengine.com/4.26/ko/ProgrammingAndScripting/ClassCreation/) 으로 설계해 불필요한 코드의 낭비를 줄이면서 프로그래밍한다.
- **엔진과 연결되어 소통하는 로그인,회원가입,채팅 서버는 프로젝트 폴더 이외의 별도 폴더에서 작업한다.
- **함수(또는 메소드)가 한 가지 일만 정확하게 기능하도록 구현한다.


<br>

## 📝 과제 진행 요구사항

- 미션은 [https://github.com/woowacourse/javascript-racingcar-precours](https://github.com/woowacourse/javascript-racingcar-precourse) 저장소를 fork/clone해 시작한다.
- **기능을 구현하기 전에 구현할 기능 목록을 정리해 javascript-racingcar-precourse/docs/README.md 파일에** 추가한다.
- **git의 commit 단위는 앞 단계에서 README.md 파일에 정리한 기능 목록 단위로 추가**한다.
- [프리코스 과제 제출](https://github.com/woowacourse/woowacourse-docs/tree/master/precourse) 문서 절차를 따라 미션을 제출한다.

## ✉️ 미션 제출 방법

- 미션 구현을 완료한 후 GitHub을 통해 제출해야 한다.
   - GitHub을 활용한 제출 방법은 [프리코스 과제 제출 문서](https://github.com/woowacourse/woowacourse-docs/tree/master/precourse) 를 참고해 제출한다.
- GitHub에 미션을 제출한 후 [우아한테크코스 지원 플랫폼](https://apply.techcourse.co.kr) 에 접속하여 프리코스 과제를 제출한다.
   - 자세한 방법은 [링크](https://github.com/woowacourse/woowacourse-docs/tree/master/precourse#제출-가이드) 를 참고한다.
   - **Pull Request만 보내고, 지원 플랫폼에서 과제를 제출하지 않으면 최종 제출하지 않은 것으로 처리되니 주의한다.**


### 🚨 과제 제출 전 체크리스트 - 0점 방지를 위한 주의사항
- 요구사항에 명시된 출력값 형식을 지키지 않을 경우 기능 구현을 모두 정상적으로 했더라도 0점으로 처리된다.
- 기능 구현을 완료한 뒤 아래 가이드에 따라 테스트를 실행했을 때 모든 테스트가 성공하는 지 확인한다. **테스트가 실패할 경우 0점으로 처리되므로, 반드시 확인 후 제출한다.**

### ✔️ 테스트 실행 가이드
- 테스트 실행에 필요한 패키지 설치를 위해 `Node.js` 버전 `14` 이상이 필요하다. 
- 다음 명령어를 입력해 패키지를 설치한다.     
```bash
// {폴더 경로}/javascript-racingcar-precourse/ 에서
npm install
```     

- 설치가 완료되었다면, 다음 명령어를 입력해 테스트를 실행한다.
```bash
// {폴더 경로}/javascript-racingcar-precourse/ 에서
npm run test
```

- 아래와 같은 화면이 나오며 모든 테스트가 pass한다면 성공! 

![테스트 결과](./images/test_result.png)
