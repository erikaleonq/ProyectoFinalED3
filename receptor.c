#include "receptor.h"

char received_message[5];
int received_message_length = 0;
uint16_t converted_value = 0;
bool tag_detected = false;
uint16_t cuenta = 0;
uint16_t precio = 0;

void clear_uart_buffer(uart_inst_t *uart)
{
    while (uart_is_readable(uart))
    {
        char c = uart_getc(uart);
    }
}

void init_uart()
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);
}

void recive_tag()
{

    if (uart_is_readable(UART_ID))
    {
        char received_char = uart_getc(UART_ID);
        sleep_ms(1);

        received_message[received_message_length] = received_char;
        received_message[received_message_length + 1] = '\0';
        received_message_length++;
    }
    else if (received_message_length > 0)
    {
        converted_value = atof(received_message);
        received_message[0] = '\0'; // Reiniciar el mensaje
        received_message_length = 0;
        tag_detected = true;
    }
}

void product_exist() {
    bool tag_exists = false;
    uint16_t id;
    id = converted_value;

    if (tag_detected) {
        for (int i = 0; i < MAX_POS; i++)
        {
            DatosFlotantes datosLeidos = i2c_read_struct(EEPROM_ADDR, i);

            // printf("valor1: %f\n", datosLeidos.valor1);
            if (datosLeidos.valor1 == converted_value) {
                printf("ID: %li, Precio: %li, Cantidad %li\n", datosLeidos.valor1, datosLeidos.valor2, datosLeidos.valor3);
                if (datosLeidos.valor3 > 0)
                {
                    DatosFlotantes misDatos;
                    misDatos.valor1 = datosLeidos.valor1;
                    misDatos.valor2 = datosLeidos.valor2;
                    misDatos.valor3 = datosLeidos.valor3 -1 ;
                    i2c_write_struct(EEPROM_ADDR, misDatos, i);

                    precio = datosLeidos.valor2;
                    cuenta += precio;
                    displayProd(datosLeidos.valor1, datosLeidos.valor2);
                    ChgLine();
                    WriteStr("TOTAL: $", 8);
                    WriteInt(cuenta*100);
                }
                else{
                    writeInfo("SIN EXISTENCIAS", 15, "  DEL PRODUCTO", 14);
                }
                tag_exists = true;
                break;
            }
            else {
                tag_exists = false;
            }
        }
        if (!tag_exists && id != 0) {
            writeInfo("    PRODUCTO", 12, "  DESCONOCIDO", 13);;
        }
        tag_detected = false;
        
    }
}

void reiniciarCuenta() {
    cuenta = 0;
}

uint16_t *getCuenta() {
    return &cuenta;
}

uint16_t getPrecio() {
    return precio;
}