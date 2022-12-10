#include "window.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <unordered_map>

/**
 * Aqui captura-se eventos do teclado e mouse para movimentação da camera
 */
void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_panSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_panSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_q)
      m_truckSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_e)
      m_truckSpeed = 1.0f;
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_q && m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_e && m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
  }
}

/**
 * No onCreate habilitamos o teste de profundidade, criamos o VBO, EBO e VAO
 * usando m_vertices e m_indices. Aqui também carregamos o arquivo do dragão.obj
 * e definimos os valores iniciais das posições, ângulos, alturas e escalas dos
 * dragões.
 */
void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_model.loadObj(assetsPath + "dragon.obj");
  m_model.setupVAO(m_program);

  /**
   *Abaixo vamos definir a escala e altura inicias dos dragões
   */
  scale = 0.1f;
  height = 0.0f;

  /**
   *Aqui define-se a posição e o ângulo inicial de cada um dos 4 dragões
   */
  dragons[0].position.x = -2.0f;
  dragons[0].position.y = height;
  dragons[0].position.z = 2.0f;
  dragons[0].angle = 180.0f;

  dragons[1].position.x = 2.0f;
  dragons[1].position.y = height;
  dragons[1].position.z = 2.0f;
  dragons[1].angle = 270.0f;

  dragons[2].position.x = 2.0f;
  dragons[2].position.y = height;
  dragons[2].position.z = -2.0f;
  dragons[2].angle = 360.0f;

  dragons[3].position.x = -2.0f;
  dragons[3].position.y = height;
  dragons[3].position.z = -2.0f;
  dragons[3].angle = 90.0f;
}

/**
 *A função onPaint() é chamada a cada frame para desenhar os dragões na tela
 */
void Window::onPaint() {
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  abcg::glBindVertexArray(m_VAO);

  /**
   * Chamamos a função drawDragon para cada dos 4 dragões passando a posição do
   * dragão no array e sua cor rgb
   */
  drawDragon(0, 1.0f, 1.0f, 1.0f);
  drawDragon(1, 0.0f, 1.0f, 1.0f);
  drawDragon(2, 1.0f, 0.0f, 1.0f);
  drawDragon(3, 1.0f, 1.0f, 0.0f);

  abcg::glBindVertexArray(0);

  // Draw ground
  m_ground.paint();
  m_model.render();
  abcg::glUseProgram(0);
}

/**
 * Aqui desenha-se os dragões de acordo com suas posições, ângulos, alturas,
 * escalas e cores
 */
void Window::drawDragon(int i, float color_r, float color_g, float color_b) {
  glm::mat4 model{1.0f};
  model = glm::translate(
      model, glm::vec3(dragons[i].position.x, height, dragons[i].position.z));
  model =
      glm::rotate(model, glm::radians(dragons[i].angle), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(scale));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, color_r, color_g, color_b, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);
}

/**
 * Aqui criamos a interface que permite ao usuário pausar o jogo, alterar as
 *alturas, escalas e velocidades de movimento e rotação dos dragões.
 */
void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  auto const widgetSize{ImVec2(218, 180)};
  ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
  ImGui::SetNextWindowSize(widgetSize);
  ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

  {
    ImGui::RadioButton("start", &startGame, 1);
    ImGui::SameLine();
    ImGui::RadioButton("pause", &startGame, 0);
    ImGui::SliderFloat("Vel. Movimento", &MovementVelocity, 0.0f, 5.0f, "%.1f");
    ImGui::SliderFloat("Vel. Rotação", &RotationVelocity, 0.0f, 200.0f, "%.1f");
    ImGui::SliderFloat("Escala", &scale, 0.1f, 0.2f, "%.2f");
    ImGui::SliderFloat("Altura", &height, 0.0f, 1.0f, "%.2f");
  }
  ImGui::End();
}

/**
 * Função executada a cada frame responsável por chamar a função
 * updateDragonPosition que altera a posição dos dragões e da câmera controlada
 * pelo usuário
 */
void Window::onUpdate() {
  updateDragonPosition(0);
  updateDragonPosition(1);
  updateDragonPosition(2);
  updateDragonPosition(3);
}

/**
 * updateDragonPosition é responsável por alterar a posição e rotação dos
 * dragões. Aqui também é atualizada a posição da câmera do usuário.
 */
void Window::updateDragonPosition(int i) {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  if (startGame) {
    /**
     * As variáveis X, Z e A são utilizadas somente para facilitar o
     * entendimento na hora de utilizar nos if's.
     */
    float X = dragons[i].position.x;
    float Z = dragons[i].position.z;
    float A = dragons[i].angle;

    // Rotacionando no Ponto A
    if (X <= -2 && Z >= 2 && A <= 180.0f) {
      A += deltaTime * RotationVelocity;
    }

    // Movimentando do Ponto A ao Ponto B
    else if (X <= 2 && Z >= 2 && A >= 180.0f) {
      X += deltaTime * MovementVelocity;
    }

    // Rotacionando no Ponto B
    if (X >= 2 && Z >= 2 && A <= 270.0f) {
      A += deltaTime * RotationVelocity;
    }

    // Movimentando do Ponto B ao Ponto C
    else if (X >= 2 && Z >= -2 && A >= 270.0f) {
      Z -= deltaTime * MovementVelocity;
    }

    // Rotacionando no Ponto C
    else if (X >= 2 && Z <= -2 && A <= 360.0f) {
      A += deltaTime * RotationVelocity;
    }

    // Movimentando do Ponto C ao Ponto D
    else if (X >= -2 && Z <= -2 && A >= 360) {
      X -= deltaTime * MovementVelocity;
    }

    // Ao chegar no Ponto D definimos o ângulo = 0, pois 360° == 0°
    else if (X <= -2 && Z <= -2 && A >= 360) {
      A = 0.0f;
    }

    // Rotacionando no Ponto D
    else if (X <= -2 && Z <= -2 && A <= 90.0f) {
      A += deltaTime * RotationVelocity;
    }

    // Movimentando do ponto D ao Ponto A
    else if (X <= -2 && Z <= 2 && A >= 90.0f) {
      Z += deltaTime * MovementVelocity;
    }

    /**
     * Atualizando posições e ângulos com os novos valores
     */
    dragons[i].position.x = X;
    dragons[i].position.z = Z;
    dragons[i].angle = A;
  }

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy() {
  m_ground.destroy();

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}
