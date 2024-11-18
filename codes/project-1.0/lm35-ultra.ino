/* ;;;;;;;;;;;;;;;;;;;;;==[ AUTORES ]==;;;;;;;;;;;;;;;;;;;;;;;
 * 
 * // Nome: Ryan David dos Santos Silvestre
 * // Nome: Thiago Freire
 * 
 * ;;;;;;;;;;;;;;;;;;;;;==[ INFORMAÇÕES ACADÊMICAS ]==;;;;;;;;;;;;;;;;;;;;;;;
 * 
 * //==[ Universidade Federal do Rio Grande do Norte ]
 * // IMD0901 - Introdução à Internet das Coisas
 * // Professor: Leonardo Aquino
 * 
 * ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ==[ INTRODUÇÃO ]== ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 * 
 * O presente código é a configuração de um monitoramento térmico que tem por objetivo
 * evitar o superaquecimento por longos períodos de tempo de uma CPU de computador.
 * Para tanto, foram utilizados um sensor de temperatura LM35 e um sensor ultrassônico HC-SR04 para
 * a realização das principais medições.
 * Como componentes adicionais, temos o LED, o push-button e um buzzer.
 * 
 * ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*/

#define LM35 A0

#define HCTRIG 13
#define HCECHO 12
#define BUZZER 4
#define LED 5
#define BUTTON 2
#define SOUND_SPEED 0.034

const float MAX_TEMP = 30.0;
const float MAX_DIST = 50.0;

void setup() {
    Serial.begin(9600);
    pinMode(HCTRIG, OUTPUT);
    pinMode(HCECHO, INPUT);
    pinMode(LM35, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
}

float lerTemperatura() {
    int valorLeitura = analogRead(LM35);
    float tensao = (valorLeitura * 5.0) / 1023.0;
    float temperatura = tensao * 100.0;
    Serial.print("Temperatura > ");
    Serial.println(temperatura);
    return temperatura;
}

float lerDistancia() {
    digitalWrite(HCTRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(HCTRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(HCTRIG, LOW);
    return pulseIn(HCECHO, HIGH) * SOUND_SPEED / 2.0;
}

void perturbarUsuarioProximo(bool botaoFoiPressionado, unsigned long tempoInicio) {
    if (!botaoFoiPressionado) {
        int tempoDecorrido = millis() - tempoInicio;
        int frequencia = max(100, 1000 - tempoDecorrido / 10);
        digitalWrite(LED, HIGH);
        digitalWrite(BUZZER, HIGH);
        delay(frequencia);
        digitalWrite(LED, LOW);
        digitalWrite(BUZZER, LOW);
        delay(frequencia);
    }
}

void perturbarUsuarioDistante(bool botaoFoiPressionado, unsigned long tempoInicio) {
    if (!botaoFoiPressionado) {
        int tempoDecorrido = millis() - tempoInicio;
        int frequencia = max(100, 1000 - tempoDecorrido / 5);
        digitalWrite(BUZZER, HIGH);
        delay(frequencia);
        digitalWrite(BUZZER, LOW);
        delay(frequencia);
    }
}

void loop() {
    float temperatura = lerTemperatura();
    float distancia = lerDistancia();

    bool usuarioProximo = (distancia < MAX_DIST);
    bool botaoPressionado = digitalRead(BUTTON) == LOW;

    if (temperatura >= MAX_TEMP) {
        unsigned long tempoInicio = millis();

        if (!usuarioProximo) {
            digitalWrite(LED, HIGH);
            while (!botaoPressionado) {
                perturbarUsuarioDistante(botaoPressionado, tempoInicio);
                botaoPressionado = digitalRead(BUTTON) == LOW;

                if (botaoPressionado) {
                    digitalWrite(LED, LOW);
                    break;
                }
            }
        } else {
            while (usuarioProximo) {
                perturbarUsuarioProximo(botaoPressionado, tempoInicio);
                botaoPressionado = digitalRead(BUTTON) == LOW;
                distancia = lerDistancia();
                usuarioProximo = (distancia < MAX_DIST);

                if (botaoPressionado) {
                    break;
                }
            }
        }
    } else {
        digitalWrite(LED, LOW);
        digitalWrite(BUZZER, LOW);
    }
}
