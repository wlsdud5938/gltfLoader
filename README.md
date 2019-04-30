# gltfLoader

# gltfLoader
tinygltf를 활용한 간단한 gltfLoader프로그램 입니다.
질문사항은 jinyoung@kookmin.ac.kr로 메일 보내주세요

# gltfLoader 시작
git을 clone 받고 clone 받은 디렉토리에 아래의 디렉토리들을 확인하고 각각을 컴파일합니다.
- loader
- saver
- viewer

확인 후 make를 통해 컴파일 합니다.
<pre><code> $ make </pre></code>

# gltf 구조 이해하기
gltf는 크로노스 그룹에서 만든 JSON기반의 3D scene 및 model 파일입니다.
gltf의 버전은 1.0과 2.0이 존재하지만 우리는 2.0을 사용합니다.
gltf의 더 자세한 내용은 https://github.com/KhronosGroup/glTF 를 참조하세요

![image1](./image/image.png)

위의  그림은 gltf파일의 구조도입니다.
gltf파일을 읽으면 위의 구조도를 참고하여 원하는 component로 접근하면 됩니다.

# gltfLoader 이해하기
gltfLoader에는 3가지 프로그램이 존재합니다.
- loader
- saver
- viewer

## Loader
gltf 파일을 읽고 bytedata를 float형태로 바꾸어 출력해 줍니다.
gltf 파일 내의 정보들을 얻을 수 있습니다.

![image1](./image/loader.png)

## Saver
gltf 파일을 읽고 사용자가 지정한 데이터를 입력하면 입력한 데이터에 맞는 gltf모델을 생성하여 저장합니다.
사용자가 지정해야할 데이터는 vertexCount, index, vertexPosition, normal, color 등 입니다.

![image1](./image/saver.png)

saver를 통해 생성한 모델입니다.

![image1](./image/makeImg.png)

## Viewer
gltf파일을 읽고 보여주는 프로그램입니다.
<pre><code> $ ./basic_viewer /gltf파일경로 </pre></code>
를 실행시키면 해당 gltf모델이 보여집니다.

![image1](./image/viewer.png)