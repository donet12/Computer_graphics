#include "scene.h"
#include "binary/animation.h"
#include "binary/skeleton.h"
#include "binary/player.h"

Shader* Scene::vertexShader = nullptr;
Shader* Scene::fragmentShader = nullptr;
Program* Scene::program = nullptr;
Camera* Scene::camera = nullptr;
Object* Scene::player = nullptr;
Texture* Scene::diffuse = nullptr;
Texture* Scene::normal = nullptr;
Material* Scene::material = nullptr;
Object* Scene::lineDraw = nullptr;
Texture* Scene::lineColor = nullptr;
Material* Scene::lineMaterial = nullptr;

vector<vec3>* playertangents = nullptr;

bool Scene::buttonFlag = true;

void Scene::setup(AAssetManager* aAssetManager) {
    Asset::setManager(aAssetManager);

    Scene::vertexShader = new Shader(GL_VERTEX_SHADER, "vertex.glsl");
    Scene::fragmentShader = new Shader(GL_FRAGMENT_SHADER, "fragment.glsl");

    Scene::program = new Program(Scene::vertexShader, Scene::fragmentShader);

    Scene::camera = new Camera(Scene::program);
    Scene::camera->eye = vec3(0.0f, 0.0f, 80.0f);

    Scene::diffuse = new Texture(Scene::program, 0, "colorMap", playerTexels, playerSize);
    Scene::material = new Material(Scene::program, diffuse);

    playertangents = new vector<vec3>(playerVertices.size(), vec3(0.0f));
    Scene::player = new Object(program, material, playerVertices, playerIndices, *playertangents);

    player->calculateTangents(*playertangents);
    player->load(playerVertices, playerIndices, *playertangents);
    player->worldMat = scale(vec3(1.0f / 3.0f));

    Scene::normal = new Texture(Scene::program, 1, "normalMap", playerNormal, playerSize);
    normal->update();


    //Scene::lineColor = new Texture(Scene::program, 0, "ColorMap", {{0xFF, 0x00, 0x00}}, 1);
    // Scene::lineMaterial = new Material(Scene::program, lineColor);
    // Scene::lineDraw = new Object(program, lineMaterial, {{}}, {{}}, GL_LINES);

    // provide lightDir variable
    GLint lightDirtLoc = glGetUniformLocation(program->get(), "lightDir");
    if (lightDirtLoc >= 0) glUniform3f(lightDirtLoc, camera->eye.x,camera->eye.y, camera->eye.z);
}

void Scene::screen(int width, int height) {
    Scene::camera->aspect = (float) width/height;
}

void Scene::update(float deltaTime) {
    Scene::program->use();
    Scene::camera->update();

    static float elapsedTime = 0.0f;
    elapsedTime = fmod((elapsedTime + deltaTime), 4.0f); // 시간을 4초간격으로 반복시키도록 설정
    static std::vector<glm::mat4> lastFixedTransforms(jNames.size(), glm::mat4(1.0f));//각 관절의 변환 행렬을 저장하기 위한 벡터
    vector<mat4> Mia; //애니메이션된 포즈 4x4행렬을 저장하는 벡터 정의
    vector<mat4> Mip; // 기본 디폴트 포즈 4x4행렬을 저장하는 벡터 정의
    vector<mat4> Mid; // 기본 디폴트 포즈 4x4행렬의 역행렬을 저장하는 벡터 정의
    vector<mat4> palette; //스키닝 과정에서 필요한 팔레트 정의

    // 보간하기
    vector<float> currentFrame = motions[(int)elapsedTime]; //현재 시간에 해당하는 모션을 가져옴
    vector<float> nextFrame = motions[((int)elapsedTime + 1) % 4]; // 4개프레임으로 반복되게 다음 시간에 해당하는 모션을 가져옴
    float frameFraction = elapsedTime -(int)elapsedTime; //프레임사이의 보간비율 계산히기위해 사용

    for (int i=0;i<jOffsets.size();i++) {
        Mip.push_back(glm::translate(jOffsets[i])); // jOffset모든 요소에대해 변환행렬을 적용하여 Mip에 저장
    }
    for (int i = 0; i < jNames.size(); i++) {
        if (i == 0) {
            Mid.push_back((inverse(Mip[i]))); // 루트 뼈의 기본 포즈 행렬의 역행렬을 저장 (Mid=Mip^-1)
        } else {
            Mid.push_back((inverse(Mip[i])) * Mid[jParents[i]]); //루트 뼈가 아니면 자식의 기본 포즈 역행렬과 부모의 기본 포즈 역행렬을 곱하여 저장 (Mid=Mip^-1 * M(i-1)d^-1)

        }
    }
    mat4 rotation;
    for(int i =0; i<jNames.size(); i++) {

        //현재 프레임과 다음 프레임의 회전 각도를 라디안으로 변환
        vec3 currentAngles = vec3(
                radians(currentFrame[i * 3 + 3]),
                radians(currentFrame[i * 3 + 4]),
                radians(currentFrame[i * 3 + 5])
        );
        vec3 nextAngles = vec3(
                radians(nextFrame[i * 3 + 3]),
                radians(nextFrame[i * 3 + 4]),
                radians(nextFrame[i * 3 + 5])
        );

        //현재프레임과 다음프레임을 ZXY순서로 회전행렬을 곱함
        mat4 current = glm::rotate(currentAngles.z, vec3(0.0f, 0.0f, 1.0f)) *
                       rotate(currentAngles.x, vec3(1.0f, 0.0f, 0.0f)) *
                       rotate(currentAngles.y, vec3(0.0f, 1.0f, 0.0f));

        mat4 next = glm::rotate(nextAngles.z, vec3(0.0f, 0.0f, 1.0f)) *
                    rotate(nextAngles.x, vec3(1.0f, 0.0f, 0.0f)) *
                    rotate(nextAngles.y, vec3(0.0f, 1.0f, 0.0f));

        //현재프레임과 다음프레임의 회전행렬을 쿼터니언으로 변환
        quat rotationCurrent = glm::quat_cast(current);
        quat rotationNext = glm::quat_cast(next);


        //buttonFlag가 true 또는 buttonFlag가 false 이면서 head, neck, headnub이 아닐경우 회전행렬보간
        if (buttonFlag || !(jNames[i] == "Neck" || jNames[i] == "Head" || jNames[i] == "HeadNub")) {
            rotation = glm::mat4_cast(glm::slerp(rotationCurrent, rotationNext, frameFraction));
            lastFixedTransforms[i] = rotation; // 업데이트된 변환을 저장
        } else {
            rotation = lastFixedTransforms[i]; // 고정된 변환을 사용
        }

        if (i == 0) {
            //평행이동 mix를 사용하여 첫 6개의 루트 프레임 보간
            mat4 translation = translate(
                    mix(vec3(currentFrame[0], currentFrame[1], currentFrame[2]),
                        vec3(nextFrame[0], nextFrame[1], nextFrame[2]),
                        frameFraction));

            Mia.push_back(Mip[i] * translation * rotation); //기본디폴트 포즈 행렬과 평행이동과 회전행렬을 곱하여 첫 애니메이션포즈에 저장
        }else {
            Mia.push_back(Mia[jParents[i]] * Mip[i] * rotation); // 부모 애니메이션포즈와 자식 디폴트포즈와 회전행렬을 곱하여 자식 애니메이션 포즈에 저장 (Mia=M(i-1)a* Mip * Mil)
        }

    }


    for (int i = 0; i < jNames.size(); i++) {
        palette.push_back(Mia[i] * Mid[i]); // 애니메이션된 포즈와 디폴트 포즈의 역행렬을 곱한 최종 변환 행렬을 팔레트에 저장 (palette = Mia * Mid)
    }
    //스키닝
    static vector<Vertex> animatedPlayer = playerVertices; //정점의 원본 데이터를 저장하기위해 animatedPlayer 사용

    for (int i = 0; i < playerVertices.size(); i++) {
        Vertex V =animatedPlayer[i]; //현재 정점을 가져옴
        vec3 skinnedPosition ={0.0f,0.0f,0.0f}; //변환된 위치 저장할 벡터 초기화
        for (int j = 0; j < 4; j++) {
            if (V.bone[j] != -1)
            skinnedPosition  += V.weight[j] * vec3(palette[V.bone[j]]*vec4(V.pos,1.0f));//해당 정점의 위치와 뼈의 최종 변환행렬을 곱하고 정점의가중치를 적용하여 변환된 위치를 누적
        }

        playerVertices[i].pos = skinnedPosition ;  //최종 변환된 위치를 원본데이터에 저장

    }

    // Line Drawer
    // glLineWidth(20);
    // Scene::lineDraw->load({{vec3(-20.0f, 0.0f, 0.0f)}, {vec3(20.0f, 0.0f, 0.0f)}}, {0, 1});
    // Scene::lineDraw->draw();

    player->calculateTangents(*playertangents);
    Scene::player->load(playerVertices, playerIndices, *playertangents);
    Scene::player->draw();

}

void Scene::setButtonFlag(bool flag)
{
    Scene::buttonFlag = flag;
}