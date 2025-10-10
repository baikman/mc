switch (command) {
    case 16:
        PORTD = 0xFF;
        break;
    case 17:
        PORTD = 0x00;
        break;
    case 18:
        PORTD = PORTD << 1;
        break;
    case 19:
        PORTD = (PORTD >> 1) + 0x80;
        break;
    case 96:
        PORTD = 0xAA;
        break;
    default:
        PORTD = 0x99;
        break;
}