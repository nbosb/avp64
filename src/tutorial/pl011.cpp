#include "tutorial/pl011.h"

namespace tutorial {

/*
 * When the pl011 class contains a vcml::serial_target_socket, this function is
 * called by vcml::serial_host every time a new input byte arrives
 */
void pl011::serial_receive(vcml::u8 data) {
    if (is_enabled() || is_rx_enabled()) {
        if (m_fifo.size() < m_fifo_size)
            m_fifo.push(data);
        update();
    }
}

/*
 * This function is called every time the Data Register `dr` is read
 */
vcml::u16 pl011::read_dr() {
    vcml::u16 val = 0;

    /// TODO: check if the fifo contains any new values. If yes, pop a value
    /// and store it in `val`

    dr = val;

    // set flags in Receive Status Register
    rsr = (val >> RSR_O) & RSR_M;

    // update flags and interrupts
    update();
    return val;
}

/*
 * This function is called on every write to the Data Register `dr`. A write
 * means that a character should be printed. The lower 8 bits of the parameter
 * `val` contian the character that should be printed. The upper 8 bits are
 * used to signal transmission errors. Since those errors are not simulated,
 * the upper 8 bits can be ignored.
 */
void pl011::write_dr(vcml::u16 val) {
    if (!is_tx_enabled())
        return;

    /// TODO: ignore the upper 8 bits of `val` and store the character in the
    /// Data Register `dr`

    // set the transmit interrupt bit
    ris |= RIS_TX;

    /// TODO: send the character to the terminal using the serial initiator
    /// socket

    // update flags and interrupts
    update();
}

void pl011::update() {
    // update flags
    fr &= ~(FR_RXFE | FR_RXFF | FR_TXFF);
    fr |= FR_TXFE; // tx FIFO is always empty
    if (m_fifo.empty())
        fr |= FR_RXFE;
    if (m_fifo.size() >= m_fifo_size)
        fr |= FR_RXFF;

    // update interrupts
    if (m_fifo.empty())
        ris &= ~RIS_RX;
    else
        ris |= RIS_RX;
    mis = ris & imsc;
    if (mis != 0 && !irq.read())
        log_debug("raising interrupt");
    if (mis == 0 && irq.read())
        log_debug("clearing interrupt");
    irq.write(mis != 0);
}

void pl011::write_rsr(vcml::u8 val) {
    //  A write to this register clears the framing, parity, break,
    //  and overrun errors. The data value is not important.
}

void pl011::write_ibrd(vcml::u16 val) {
    ibrd = val & LCR_IBRD_M;
}

void pl011::write_fbrd(vcml::u16 val) {
    fbrd = val & LCR_FBRD_M;
}

void pl011::write_lcr(vcml::u8 val) {
    if ((val & LCR_FEN) && !(lcr & LCR_FEN))
        log_debug("FIFO enabled");
    if (!(val & LCR_FEN) && (lcr & LCR_FEN))
        log_debug("FIFO disabled");

    m_fifo_size = (val & LCR_FEN) ? FIFOSIZE : 1;

    lcr = val & LCR_H_M;
}

void pl011::write_cr(vcml::u16 val) {
    if (!is_enabled() && (val & CR_UARTEN))
        log_debug("device enabled");
    if (is_enabled() && !(val & CR_UARTEN))
        log_debug("device disabled");
    if (!is_tx_enabled() && (val & CR_TXE))
        log_debug("transmitter enabled");
    if (is_tx_enabled() && !(val & CR_TXE))
        log_debug("transmitter disabled");
    if (!is_rx_enabled() && (val & CR_RXE))
        log_debug("receiver enabled");
    if (is_rx_enabled() && !(val & CR_RXE))
        log_debug("receiver disabled");

    cr = val;
}

void pl011::write_ifls(vcml::u16 val) {
    ifls = val & 0x3f;
}

void pl011::write_imsc(vcml::u16 val) {
    imsc = val & RIS_M;
    update();
}

void pl011::write_icr(vcml::u16 val) {
    ris &= ~(val & RIS_M);
    icr = 0;
    update();
}

pl011::pl011(const sc_core::sc_module_name& nm):
    peripheral(nm),
    serial_host(),
    m_fifo_size(),
    m_fifo(),
    /// TODO: initialize the Data Register `dr`
    rsr("rsr", 0x004, 0x0),
    fr("fr", 0x018, FR_TXFE | FR_RXFE),
    ilpr("ilpr", 0x020, 0x0),
    ibrd("ibrd", 0x024, 0x0),
    fbrd("fbrd", 0x028, 0x0),
    lcr("lcr", 0x02C, 0x0),
    cr("cr", 0x030, CR_TXE | CR_RXE),
    ifls("ifls", 0x034, 0x0),
    imsc("imsc", 0x038, 0x0),
    ris("ris", 0x03C, 0x0),
    mis("mis", 0x040, 0x0),
    icr("icr", 0x044, 0x0),
    dmac("dmac", 0x048, 0x0),
    pid("pid", 0xFE0, 0x00),
    cid("cid", 0xFF0, 0x00)
/// TODO: initialize the tlm target socket
/// TODO: initialize the gpio irq socket
/// TODO: initialize the serial target and initiator sockets
{
    /// TODO: sync local time and simulation time on every read/write of the
    /// Data Register `dr`

    /// TODO: allows to read and write the Data Register `dr`

    /// TODO: on every read of the Data Register `dr`, call the function
    /// `pl011::read_dr`

    /// TODO: on every write to the Data Register `dr`, call the function
    /// `pl011::write_dr`

    rsr.sync_always();
    rsr.allow_read_write();
    rsr.on_write(&pl011::write_rsr);

    fr.sync_always();
    fr.allow_read_only();

    ilpr.sync_never();
    ilpr.allow_read_write(); // not implemented

    ibrd.sync_always();
    ibrd.allow_read_write();
    ibrd.on_write(&pl011::write_ibrd);

    fbrd.sync_always();
    fbrd.allow_read_write();
    fbrd.on_write(&pl011::write_fbrd);

    lcr.sync_always();
    lcr.allow_read_write();
    lcr.on_write(&pl011::write_lcr);

    cr.sync_always();
    cr.allow_read_write();
    cr.on_write(&pl011::write_cr);

    ifls.sync_always();
    ifls.allow_read_write();
    ifls.on_write(&pl011::write_ifls);

    imsc.sync_always();
    imsc.allow_read_write();
    imsc.on_write(&pl011::write_imsc);

    ris.sync_always();
    ris.allow_read_only();

    mis.sync_always();
    mis.allow_read_only();

    icr.sync_always();
    icr.allow_write_only();
    icr.on_write(&pl011::write_icr);

    dmac.sync_never();
    dmac.allow_read_write(); // not implemented

    pid.sync_never();
    pid.allow_read_only();

    cid.sync_never();
    cid.allow_read_only();
}

/*
 * This function is called by vcml::peripheral when a reset request is received
 */
void pl011::reset() {
    peripheral::reset();

    for (unsigned int i = 0; i < pid.count(); i++)
        pid[i] = static_cast<vcml::u32>(AMBA_PID >> (i * 8)) & 0xff;

    for (unsigned int i = 0; i < cid.count(); i++)
        cid[i] = static_cast<vcml::u32>(AMBA_CID >> (i * 8)) & 0xff;

    while (!m_fifo.empty())
        m_fifo.pop();

    irq = false;
}

} // namespace tutorial
