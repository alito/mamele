// license: GPL-2.0+
// copyright-holders: Dirk Best
/***************************************************************************

    Einstein User Port

***************************************************************************/

#include "emu.h"
#include "userport.h"

// supported devices
#include "mouse.h"
#include "speech.h"


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(EINSTEIN_USERPORT, einstein_userport_device, "einstein_userport", "Einstein User Port")


//**************************************************************************
//  SLOT DEVICE
//**************************************************************************

//-------------------------------------------------
//  einstein_userport_device - constructor
//-------------------------------------------------

einstein_userport_device::einstein_userport_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, EINSTEIN_USERPORT, tag, owner, clock),
	device_single_card_slot_interface<device_einstein_userport_interface>(mconfig, *this),
	m_card(nullptr),
	m_bstb_handler(*this)
{
}

//-------------------------------------------------
//  einstein_userport_device - destructor
//-------------------------------------------------

einstein_userport_device::~einstein_userport_device()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void einstein_userport_device::device_start()
{
	// resolve callbacks
	m_bstb_handler.resolve_safe();

	m_card = get_card_device();
}


//**************************************************************************
//  I/O PORTS
//**************************************************************************

uint8_t einstein_userport_device::read()
{
	if (m_card)
		return m_card->read();
	else
		return 0xff;
}

void einstein_userport_device::write(uint8_t data)
{
	if (m_card)
		m_card->write(data);
}

WRITE_LINE_MEMBER( einstein_userport_device::brdy_w )
{
	if (m_card)
		m_card->brdy_w(state);
}


//**************************************************************************
//  CARTRIDGE INTERFACE
//**************************************************************************

//-------------------------------------------------
//  device_einstein_userport_interface - constructor
//-------------------------------------------------

device_einstein_userport_interface::device_einstein_userport_interface(const machine_config &mconfig, device_t &device) :
	device_interface(device, "einsteinuser")
{
	m_slot = dynamic_cast<einstein_userport_device *>(device.owner());
}

//-------------------------------------------------
//  ~device_einstein_userport_interface - destructor
//-------------------------------------------------

device_einstein_userport_interface::~device_einstein_userport_interface()
{
}


//**************************************************************************
//  SLOT INTERFACE
//**************************************************************************

void einstein_userport_cards(device_slot_interface &device)
{
	device.option_add("mouse", EINSTEIN_MOUSE);
	device.option_add("speech", EINSTEIN_SPEECH);
}
