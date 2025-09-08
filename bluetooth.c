#include "bluetooth.h"
#include "AT24C02.h"

/* 11.0592MHz：T=1.085us；9600bps 每位≈104.17us -> 约 96 计数；1.5位≈144 计数 */
#define T0_TH_1BIT   0xFF
#define T0_TL_1BIT   0xA0  /* 96 */
#define T0_TH_1P5    0xFF
#define T0_TL_1P5    0x70  /* 144 */

/* Timer1 1ms 装载（≈0xFC66） */
#define T1_TH_1MS    0xFC
#define T1_TL_1MS    0x66

/* 精确等待：使用 T0，调用期间保存并恢复 T0 状态 */
static void bt_t0_wait_reload(unsigned char th, unsigned char tl)
{
    unsigned char tmod_bak = TMOD;
    bit tr0_bak = TR0;
    unsigned char th0_bak = TH0;
    unsigned char tl0_bak = TL0;
    bit tf0_bak = TF0;

    TMOD = (TMOD & 0xF0) | 0x01; /* T0 mode1 */
    TR0 = 0;
    TH0 = th;
    TL0 = tl;
    TF0 = 0;
    TR0 = 1;
    while (!TF0) { /* wait */ }
    TR0 = 0;

    /* 恢复 */
    TH0 = th0_bak;
    TL0 = tl0_bak;
    TF0 = tf0_bak;
    TR0 = tr0_bak;
    TMOD = tmod_bak;
}

static void bt_wait_1bit(void)   { bt_t0_wait_reload(T0_TH_1BIT, T0_TL_1BIT); }
static void bt_wait_1p5bit(void) { bt_t0_wait_reload(T0_TH_1P5,  T0_TL_1P5 ); }

/* 等待起始位（RX 拉低），使用 T1 做 1ms 计时；整个过程暂时关闭串口中断 ES */
static bit bt_wait_start_low_ms(unsigned int timeout_ms)
{
    unsigned char tmod_bak = TMOD;
    bit tr1_bak = TR1;
    unsigned char th1_bak = TH1;
    unsigned char tl1_bak = TL1;
    bit tf1_bak = TF1;
    bit es_bak = ES;

    ES = 0; /* 暂停串口中断，避免我们改动 T1 影响 UART */
    TMOD = (TMOD & 0x0F) | 0x10; /* T1 mode1 */
    TR1 = 0;
    TH1 = T1_TH_1MS;
    TL1 = T1_TL_1MS;
    TF1 = 0;
    TR1 = 1;

    while (timeout_ms)
    {
        if (BT_RX == 0)
        {
            /* 恢复 T1 与 ES */
            TR1 = 0;
            TH1 = th1_bak; TL1 = tl1_bak; TF1 = tf1_bak; TR1 = tr1_bak; TMOD = tmod_bak;
            ES = es_bak;
            return 1;
        }
        if (TF1)
        {
            TF1 = 0;
            TH1 = T1_TH_1MS;
            TL1 = T1_TL_1MS;
            timeout_ms--;
        }
    }

    TR1 = 0;
    TH1 = th1_bak; TL1 = tl1_bak; TF1 = tf1_bak; TR1 = tr1_bak; TMOD = tmod_bak;
    ES = es_bak;
    return 0;
}

/* 文件内私有：8 位求和校验（注意：不要用 data 作为形参名，避免与 C51 关键字冲突） */
static unsigned char bt_checksum8(const unsigned char* buf, unsigned char len)
{
    unsigned int i, sum = 0;
    for (i = 0; i < len; i++) sum += buf[i];
    return (unsigned char)(sum & 0xFF);
}

void BT_Init(void)
{
    BT_TX = 1;      /* 空闲高电平 */
    delay_ms(500);  /* 模块上电稳定 */
}

/* 严格 9600bps 发送一个字节（LSB first） */
void BT_SendByte(unsigned char dat)
{
    bit ea_bak = EA;
    unsigned char i;

    EA = 0;         /* 防止中断抖动影响位时序 */
    /* 起始位 */
    BT_TX = 0;      bt_wait_1bit();
    /* 8 数据位 */
    for (i = 0; i < 8; i++)
    {
        BT_TX = (dat & 0x01) ? 1 : 0;
        bt_wait_1bit();
        dat >>= 1;
    }
    /* 停止位 */
    BT_TX = 1;      bt_wait_1bit();
    EA = ea_bak;
}

void BT_SendBuffer(const unsigned char* buf, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i++) BT_SendByte(buf[i]);
}

void BT_SendString(unsigned char *str)
{
    while (*str) BT_SendByte(*str++);
}

/* 带超时接收一个字节；检测起始位 -> 1.5位对齐 -> 逐位采样 */
bit BT_RecvByteTimeout(unsigned char* out, unsigned int timeout_ms)
{
    unsigned char i, val = 0;
    bit ea_bak;

    if (!bt_wait_start_low_ms(timeout_ms)) return 0;

    ea_bak = EA;
    EA = 0;
    bt_wait_1p5bit(); /* 对齐位中心 */

    for (i = 0; i < 8; i++)
    {
        /* 采样 LSB first：右移，最高位承接当前位 */
        val >>= 1;
        if (BT_RX) val |= 0x80;
        bt_wait_1bit();
    }
    /* 丢弃停止位时隙 */
    bt_wait_1bit();
    EA = ea_bak;

    *out = val;
    return 1;
}

/* 构造并发送 CARD 帧 */
bit BT_SendCardFrame(const CARD* card)
{
    unsigned char frame[BT_FRAME_LEN];
    unsigned char i, cks, *p = frame;

    *p++ = BT_HDR0;
    *p++ = BT_HDR1;
    *p++ = BT_TYPE_CARD;
    *p++ = BT_PAYLOAD_LEN;
    for (i = 0; i < ID_LEN; i++)    *p++ = card->id[i];
    for (i = 0; i < STUID_LEN; i++) *p++ = card->stuid[i];
    cks = bt_checksum8(&frame[2], (unsigned char)(1 + 1 + BT_PAYLOAD_LEN));
    *p++ = cks;

    BT_SendBuffer(frame, sizeof(frame));
    return 1;
}

/* 在 timeout_ms 内接收一帧 CARD */
bit BT_RecvCardFrame(CARD* out, unsigned int timeout_ms)
{
    unsigned char b, hdr = 0;
    unsigned char type, len, i;
    unsigned char payload[BT_PAYLOAD_LEN];
    unsigned char cks, cks_calc;

    /* 逐字节接收，带“每字节预算” */
    while (timeout_ms)
    {
        unsigned int budget = (timeout_ms > 200) ? 200 : timeout_ms;
        if (!BT_RecvByteTimeout(&b, budget)) return 0;
        timeout_ms = (budget <= timeout_ms) ? (timeout_ms - budget) : 0;

        if (!hdr)
        {
            if (b == BT_HDR0) hdr = 1;
        }
        else
        {
            if (b == BT_HDR1) break;
            else hdr = (b == BT_HDR0) ? 1 : 0;
        }
    }
    if (!timeout_ms) return 0;

    /* Type */
    if (!BT_RecvByteTimeout(&type, 200)) return 0;
    /* Len */
    if (!BT_RecvByteTimeout(&len, 200)) return 0;
    if (type != BT_TYPE_CARD || len != BT_PAYLOAD_LEN) return 0;

    /* Payload */
    for (i = 0; i < BT_PAYLOAD_LEN; i++)
    {
        if (!BT_RecvByteTimeout(&payload[i], 200)) return 0;
    }
    /* Checksum */
    if (!BT_RecvByteTimeout(&cks, 200)) return 0;

    cks_calc  = bt_checksum8(&type, 1);
    cks_calc += bt_checksum8(&len, 1);
    cks_calc += bt_checksum8(payload, BT_PAYLOAD_LEN);
    if (cks_calc != cks) return 0;

    /* 回填 CARD */
    for (i = 0; i < ID_LEN; i++) out->id[i] = payload[i];
    for (i = 0; i < STUID_LEN; i++) out->stuid[i] = payload[ID_LEN + i];

    return 1;
}

/* 十六进制发送（调试友好，不依赖字符串结束符） */
static void bt_send_hex(const unsigned char* buf, unsigned char len)
{
    unsigned char i;
    for (i = 0; i < len; i++)
    {
        unsigned char hi = (buf[i] >> 4) & 0x0F;
        unsigned char lo = buf[i] & 0x0F;
        BT_SendByte(hi < 10 ? ('0' + hi) : ('A' + hi - 10));
        BT_SendByte(lo < 10 ? ('0' + lo) : ('A' + lo - 10));
    }
}

void BT_SendCardInfo(void)
{
    unsigned char i;
    CARD card;

    BT_SendString("Card Info in EEPROM:\r\n");
    for (i = 0; i < 3; i++)
    {
        AT24C02_ReadCard_pos(i, &card);

        BT_SendString("Card ");
        BT_SendByte('1' + i);
        BT_SendString(": ID=");
        bt_send_hex(card.id, ID_LEN);
        BT_SendString(", STU=");
        bt_send_hex(card.stuid, STUID_LEN);
        BT_SendString("\r\n");

        delay_ms(50);
    }
    BT_SendString("Transfer complete.\r\n");
}