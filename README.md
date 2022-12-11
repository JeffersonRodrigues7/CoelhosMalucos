# Taz Maluco

### Henrique Augusto de Oliveira, 11065516

### Jefferson Leite Rodrigues Dantas, 11076316

---

### O que é

O projeto é uma aplicação interativa 3D. Têm-se um objeto do Taz maior no meio da tela com vários taz menores indo de um lado para o outro ao fundo.

Através da interface é possível alterar a textura, iluminação, projeção e outros aspectos dos objetos na tela.

A velocidade dos pequenos taz ao fundo pode ser modificada através de um slider.

A aplicação foi baseada nos projetos viewer6 e starfield disponibilizadas pelo professor, todas as funcionalidades utilizadas no código viewer6 estão disponíveis aqui.

---
### Histórico
Inicialmente, a dupla tentou reaproveitar o projeto da atividade 3, que era o "A Rota dos Dragões". A ideia inicial foi tentou implementar iluminação e texturização nos 4 dragões que faziam o percurso e implementar algumas correções referente ao feedback retornado para a atividade. 

O projeto chegou a compilar, mas um problema que não conseguimos corrigir foi que ficava uma tela toda preta. Não tínhamos uma log de erro ou qualquer suspeita que pudéssemos corrigir a tempo. 

A fim de cumprir com o prazo da entrega e implementar os conceitos de Iluminação e Texturização aprendidos em aula, optou-se por utilizar como base projetos vistos em aula e diferenciá-los para que pudéssemos aplicar as técnicas aprendidas e atender a entrega estabelecida.

---

### Implementação

Como a aplicação foi baseada principalmente no projeto viewer6, vamos nos ater somente as alterações feitas a partir do visto em aula.

**OBS: Os comentários serão feitos nos códigos.**

Abaixo está o que foi acrescentado no arquivo **window.hpp**.

#### window.hpp

```
  //Ângulo de rotação dos objetos de fundo
  float m_angle{};

  //Utilizada para pegarmos um valor aleatório entre um range informado
  std::default_random_engine m_randomEngine;
  /*
  * Variáveis de escala e variação da velocidade que poderão
  * ser trocadas com sliders no onPaintUI()
  */
  float scale{};
  float speed{};

  /**
  * Struct do Taz com as variáveis de posição, rotação e a direção de movimento
  */
  struct Taz {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
    glm::vec2 m_direction{};
  };

  //Array de Taz, passamos a quantidade máxima de 100
  std::array<Taz, 100> m_taz;

  //Função que randomiza posição inicial e direção de movimento dos taz's
  void randomizeTaz(Taz &Taz);
```

#### window.cpp

No **randomizeTaz** geramos as posições, rotações e direções randomizadas dos taz's.

```
void Window::randomizeTaz(Taz &taz) {

  // Gerando posições aleatórias
  std::uniform_real_distribution<float> distPosXY(-2.0f, 2.0f);
  std::uniform_real_distribution<float> distPosZ(0.0f, 0.0f);

  // Definindo posição do taz
  taz.m_position =
      glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                distPosZ(m_randomEngine));

  // Rotação do taz
  taz.m_rotationAxis = glm::sphericalRand(1.0f);

  /**
  * Vamos gerar valores randômicos entre -1 e 1 para a direção em x e y
  * Caso o valor seja 0 para as duas direções, ficaremos em um while até que
  * alguma das direções seja diferente de 0.
  */
  std::uniform_int_distribution<int> direction(-1, 1);
  taz.m_direction =
      glm::vec2(direction(m_randomEngine), direction(m_randomEngine));

  while (taz.m_direction.x == 0 && taz.m_direction.y == 0) {
    taz.m_direction =
        glm::vec2(direction(m_randomEngine), direction(m_randomEngine));
  }
}
```

No **onPaint** fazemos a renderização dos taz do fundo

```
  // Renderização dos taz
  for (auto &taz : m_taz) {
    // Computando matriz modelo do taz atual
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, taz.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
    modelMatrix = glm::rotate(modelMatrix, m_angle, taz.m_rotationAxis);

    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_model.render();
  }
```

No **onUpdate** fazemos a renderização dos taz do fundo

```
  // Calculamos o deltaTime a atualizamos o ângulo do objeto
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  // Atualizamos a posição de cada Taz de acordo com a direção e velocidade
  for (auto &taz : m_taz) {
    taz.m_position.x += taz.m_direction.x * deltaTime * speed;
    taz.m_position.y += taz.m_direction.y * deltaTime * speed;

    // Se o Taz passar do limite estipulado reiniciamos sua posição e direção de movimento chamando a função randomizeTaz
    if (taz.m_position.x > 2.0f || taz.m_position.x < -2.0f ||
        taz.m_position.y > 2.0f || taz.m_position.y < -2.0f) {
      randomizeTaz(taz);
    }
  }
```

### Referências
- **Arquivo taz.obj utilizado no projeto: ** https://www.cgtrader.com/items/1997904/download-page
- **Projeto Starfield: ** https://hbatagelo.github.io/cg/starfield.html
- **Projeto Viewer6: ** https://hbatagelo.github.io/cg/envmapping.html

### Projeto em WebAssembly
https://hadeoliveira.github.io/Projeto4ABCG/
