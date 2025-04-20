// license:BSD-3-Clause
// copyright-holders:smf
/*
 * zs01.c
 *
 * Secure SerialFlash
 *
 * This is a high level emulation of the PIC used in some of the System 573 security cartridges.
 *
 * Referred to internally in game code as "NS2K001".
 *
 */

#include "emu.h"
#include "zs01.h"

#include <cstdarg>

#define VERBOSE_LEVEL ( 0 )

inline void ATTR_PRINTF( 3, 4 ) zs01_device::verboselog( int n_level, const char *s_fmt, ... )
{
	if( VERBOSE_LEVEL >= n_level )
	{
		va_list v;
		char buf[ 32768 ];
		va_start( v, s_fmt );
		vsprintf( buf, s_fmt, v );
		va_end( v );
		logerror( "%s: zs01(%s) %s", machine().describe_context(), tag(), buf );
	}
}

// device type definition
DEFINE_DEVICE_TYPE(ZS01, zs01_device, "zs01", "Konami ZS01 PIC")

zs01_device::zs01_device( const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock ) :
	device_t(mconfig, ZS01, tag, owner, clock),
	device_nvram_interface(mconfig, *this),
	m_ds2401(*this, finder_base::DUMMY_TAG),
	m_region(*this, DEVICE_SELF),
	m_cs( 0 ),
	m_rst( 0 ),
	m_scl( 0 ),
	m_sdaw( 0 ),
	m_sdar( 0 ),
	m_state( STATE_STOP ),
	m_shift( 0 ),
	m_bit( 0 ),
	m_byte( 0 ),
	m_previous_byte( 0 )
{
}

void zs01_device::device_start()
{
	if( !m_ds2401 )
		logerror( "ds2401 '%s' not found\n", m_ds2401.finder_tag() );

	memset( m_write_buffer, 0, sizeof( m_write_buffer ) );
	memset( m_read_buffer, 0, sizeof( m_read_buffer ) );
	memset( m_response_key, 0, sizeof( m_response_key ) );

	save_item( NAME( m_cs ) );
	save_item( NAME( m_rst ) );
	save_item( NAME( m_scl ) );
	save_item( NAME( m_sdaw ) );
	save_item( NAME( m_sdar ) );
	save_item( NAME( m_state ) );
	save_item( NAME( m_shift ) );
	save_item( NAME( m_bit ) );
	save_item( NAME( m_byte ) );
	save_item( NAME( m_previous_byte ) );
	save_item( NAME( m_write_buffer ) );
	save_item( NAME( m_read_buffer ) );
	save_item( NAME( m_response_key ) );
	save_item( NAME( m_response_to_reset ) );
	save_item( NAME( m_command_key ) );
	save_item( NAME( m_data_key ) );
	save_item( NAME( m_data ) );
	save_item( NAME( m_configuration_registers ) );
}

void zs01_device::device_reset()
{
	memset( m_write_buffer, 0, sizeof( m_write_buffer ) );
	memset( m_read_buffer, 0, sizeof( m_read_buffer ) );
	memset( m_response_key, 0, sizeof( m_response_key ) );

	m_cs = 0;
	m_rst = 0;
	m_scl = 0;
	m_sdaw = 0;
	m_sdar = 0;
	m_state = STATE_STOP;
	m_shift = 0;
	m_bit = 0;
	m_byte = 0;
	m_previous_byte = 0;
}

WRITE_LINE_MEMBER( zs01_device::write_rst )
{
	if( m_rst != state )
	{
		verboselog( 2, "rst=%d\n", state );
	}

	if( m_rst == 0 && state != 0 && m_cs == 0 )
	{
		verboselog( 1, "goto response to reset\n" );
		m_state = STATE_RESPONSE_TO_RESET;
		m_bit = 0;
		m_byte = 0;
	}

	m_rst = state;
}

WRITE_LINE_MEMBER( zs01_device::write_cs )
{
	if( m_cs != state )
	{
		verboselog( 2, "cs=%d\n", state );
	}

//  if( m_cs != 0 && state == 0 )
//  {
//      /* enable chip */
//      m_state = STATE_STOP;
//  }

//  if( m_cs == 0 && state != 0 )
//  {
//      /* disable chip */
//      m_state = STATE_STOP;
//      /* high impendence? */
//      m_sdar = 0;
//  }

	m_cs = state;
}

void zs01_device::decrypt( uint8_t *destination, uint8_t *source, int length, uint8_t *key, uint8_t previous_byte )
{
	uint32_t a0;
	uint32_t v1;
	uint32_t v0;
	uint32_t a1;
	uint32_t t1;
	uint32_t t0;

	length--;
	if( length >= 0 )
	{
		do
		{
			t1 = source[ length ];
			a1 = 7;
			t0 = t1;

			do
			{
				v1 = key[ a1 ];
				a1--;
				v0 = v1 & 0x1f;
				v0 = t0 - v0;
				v1 >>= 5;
				v0 &= 0xff;
				a0 = (signed)v0 >> v1;
				v1 = 8 - v1;
				v1 &= 7;
				v0 = (signed)v0 << v1;
				t0 = a0 | v0;
			} while( a1 > 0 );

			v1 = key[ 0 ];
			a0 = previous_byte;
			v0 = t0 & 0xff;
			previous_byte = t1;
			v0 = v0 - v1;
			v0 = v0 ^ a0;

			destination[ length ] = v0;
			length--;
		}
		while( length >= 0 );
	}
}

void zs01_device::decrypt2( uint8_t *destination, uint8_t *source, int length, uint8_t *key, uint8_t previous_byte )
{
	uint32_t a0;
	uint32_t v1;
	uint32_t v0;
	uint32_t a1;
	uint32_t t2;
	uint32_t t1;
	uint32_t t0;

	t2 = 0;
	if( length >= 0 )
	{
		do
		{
			t1 = source[ t2 ];
			a1 = 7;
			t0 = t1;

			do
			{
				v1 = key[ a1 ];
				a1--;
				v0 = v1 & 0x1f;
				v0 = t0 - v0;
				v1 >>= 5;
				v0 &= 0xff;
				a0 = (signed)v0 >> v1;
				v1 = 8 - v1;
				v1 &= 7;
				v0 = (signed)v0 << v1;
				t0 = a0 | v0;
			} while( a1 > 0 );

			v1 = key[ 0 ];
			a0 = previous_byte;
			v0 = t0 & 0xff;
			previous_byte = t1;
			v0 = v0 - v1;
			v0 = v0 ^ a0;

			destination[ t2 ] = v0;
			t2++;

		} while( t2 < length );
	}
}

void zs01_device::encrypt( uint8_t *destination, uint8_t *source, int length, uint8_t *key, uint32_t previous_byte )
{
	uint32_t t0;
	uint32_t v0;
	uint32_t v1;
	uint32_t a0;
	uint32_t a1;

	length--;
	if( length >= 0 )
	{
		do
		{
			t0 = 1;
			v0 = source[ length ];
			v1 = previous_byte;
			a0 = key[ 0 ];
			v0 ^= v1;
			a0 += v0;

			do
			{
				a1 = key[ t0 ];
				t0++;
				a0 &= 0xff;
				v0 = a1 >> 5;
				v1 = a0 << v0;
				v0 = 8 - v0;
				v0 &= 7;
				a0 = (signed) a0 >> v0;
				v1 |= a0;
				v1 &= 0xff;
				a1 &= 0x1f;
				v1 += a1;
				v0 = (signed) t0 < 8;
				a0 = v1;

			} while( v0 != 0 );

			previous_byte = v1;

			destination[ length ] = a0;
			length--;

		} while( length >= 0 );
	}
}

uint16_t zs01_device::calc_crc( uint8_t *buffer, uint32_t length )
{
	uint32_t v1;
	uint32_t a3;
	uint32_t v0;
	uint32_t a2;

	v1 = 0xffff;
	a3 = 0;

	if( length > 0 )
	{
		do
		{
			v0 = buffer[ a3 ];
			a2 = 7;
			v0 = v0 << 8;
			v1 = v1 ^ v0;
			v0 = v1 & 0x8000;

			do
			{
				if( v0 != 0 )
				{
					v0 = v1 << 1;
					v1 = v0 ^ 0x1021;
				}
				else
				{
					v0 = v1 << 1;
					v1 = v1 << 1;
				}

				a2--;
				v0 = v1 & 0x8000;
			} while( (signed) a2 >= 0 );

			a3++;
			v0 = (signed) a3 < (signed) length;
		} while ( v0 != 0 );
	}

	v0 = ~v1 ;
	v0 = v0 & 0xffff;

	return v0;
}

int zs01_device::data_offset()
{
	return m_write_buffer[ 1 ] * SIZE_DATA_BUFFER;
}

WRITE_LINE_MEMBER( zs01_device::write_scl )
{
	if( m_scl != state )
	{
		verboselog( 2, "scl=%d\n", state );
	}

	if( m_cs == 0 )
	{
		switch( m_state )
		{
		case STATE_STOP:
			break;

		case STATE_RESPONSE_TO_RESET:
			if( m_scl != 0 && state == 0 )
			{
				if( m_bit == 0 )
				{
					m_shift = m_response_to_reset[ m_byte ];
					verboselog( 1, "<- response_to_reset[ %d ]: %02x\n", m_byte, m_shift );
				}

				m_sdar = ( m_shift >> 7 ) & 1;
				m_shift <<= 1;
				m_bit++;

				if( m_bit == 8 )
				{
					m_bit = 0;
					m_byte++;

					if( m_byte == sizeof( m_response_to_reset ) )
					{
						m_sdar = 1;
						verboselog( 1, "goto stop\n" );
						m_state = STATE_STOP;
					}
				}
			}
			break;

		case STATE_LOAD_COMMAND:
			// FIXME: Processing on the rising edge of the clock causes sda to change state while clock is high
			// which is not allowed. Also need to ensure that only valid device-id's and commands are acknowledged.
			if( m_scl == 0 && state != 0 )
			{
				if( m_bit < 8 )
				{
					verboselog( 2, "clock\n" );
					m_shift <<= 1;

					if( m_sdaw != 0 )
					{
						m_shift |= 1;
					}

					m_bit++;
				}
				else
				{
					m_sdar = 0;

					switch( m_state )
					{
					case STATE_LOAD_COMMAND:
						m_write_buffer[ m_byte ] = m_shift;
						verboselog( 2, "-> write_buffer[ %d ]: %02x\n", m_byte, m_write_buffer[ m_byte ] );

						m_byte++;
						if( m_byte == sizeof( m_write_buffer ) )
						{
							decrypt( m_write_buffer, m_write_buffer, sizeof( m_write_buffer ), m_command_key, 0xff );

							// TODO: What is bit 1 of m_write_buffer[0]?

							// Bit 2 seems to be set when the sector is >= 4 and the sector is not 0xfc
							if( ( m_write_buffer[ 0 ] & 4 ) != 0 )
							{
								decrypt2( &m_write_buffer[ 2 ], &m_write_buffer[ 2 ], SIZE_DATA_BUFFER, m_data_key, m_previous_byte );
							}

							uint16_t crc = calc_crc( m_write_buffer, 10 );
							uint16_t msg_crc = ( ( m_write_buffer[ 10 ] << 8 ) | m_write_buffer[ 11 ] );

							verboselog( 1, "-> command: %02x (%s)\n", m_write_buffer[ 0 ], ( m_write_buffer[ 0 ] & 1 ) ? "READ" : "WRITE" );
							verboselog( 1, "-> address: %04x (%02x)\n", data_offset(), m_write_buffer[ 1 ] );
							verboselog( 1, "-> data: %02x%02x%02x%02x%02x%02x%02x%02x\n",
								m_write_buffer[ 2 ], m_write_buffer[ 3 ], m_write_buffer[ 4 ], m_write_buffer[ 5 ],
								m_write_buffer[ 6 ], m_write_buffer[ 7 ], m_write_buffer[ 8 ], m_write_buffer[ 9 ] );
							verboselog( 1, "-> crc: %04x vs %04x %s\n", crc, msg_crc, crc == msg_crc ? "" : "(BAD)");

							if( crc == msg_crc )
							{
								m_configuration_registers[ CONFIG_RC ] = 0; // Reset password fail counter

								switch( m_write_buffer[ 0 ] & 1 )
								{
								case COMMAND_WRITE:
									memset( &m_read_buffer[ 0 ], 0, sizeof( m_write_buffer ) );
									m_read_buffer[ 0 ] = STATUS_OK;

									if ( m_write_buffer[ 1 ] == 0xfd )
									{
										// Erase
										std::fill( std::begin( m_data ), std::end( m_data ), 0 );
										std::fill( std::begin( m_data_key ), std::end( m_data_key ), 0 );
									}
									else if ( m_write_buffer[ 1 ] == 0xfe )
									{
										// Configuration register
										memcpy( m_configuration_registers, &m_write_buffer[ 2 ], SIZE_DATA_BUFFER );
									}
									else if ( m_write_buffer[ 1 ] == 0xff )
									{
										// Set password
										memcpy( m_data_key, &m_write_buffer[ 2 ], SIZE_DATA_BUFFER );
									}
									else if ( data_offset() < sizeof ( m_data ) )
									{
										memcpy( &m_data[ data_offset() ], &m_write_buffer[ 2 ], SIZE_DATA_BUFFER );
									}
									else
									{
										verboselog( 1, "-> unknown write offset: %04x (%02x)\n", data_offset(), m_write_buffer[ 1 ] );
									}

									break;

								case COMMAND_READ:
									m_read_buffer[ 0 ] = STATUS_OK;

									if ( m_write_buffer[ 1 ] == 0xfc )
									{
										// TODO: Unknown serial
										// The serial is verified by the same algorithm as the one read from 0x7e8 (DS2401 serial), but the serial is different.
										for (int i = 0; i < SIZE_DATA_BUFFER; i++)
										{
											m_read_buffer[2 + i] = m_ds2401->direct_read(SIZE_DATA_BUFFER - i - 1);
										}
									}
									else if ( m_write_buffer[ 1 ] == 0xfd )
									{
										// DS2401 serial
										/* TODO: use read/write to talk to the ds2401, which will require a timer. */
										for( int i = 0; i < SIZE_DATA_BUFFER; i++ )
										{
											m_read_buffer[ 2 + i ] = m_ds2401->direct_read( SIZE_DATA_BUFFER - i - 1 );
										}
									}
									else if ( m_write_buffer[ 1 ] == 0xfe )
									{
										// Configuration register
										memcpy( &m_read_buffer[ 2 ], m_configuration_registers, SIZE_DATA_BUFFER );
									}
									else if ( data_offset() < sizeof ( m_data ) )
									{
										memcpy( &m_read_buffer[ 2 ], &m_data[ data_offset() ], SIZE_DATA_BUFFER );
									}
									else
									{
										verboselog( 1, "-> unknown read offset: %04x (%02x)\n", data_offset(), m_write_buffer[ 1 ] );
									}

									memcpy( m_response_key, &m_write_buffer[ 2 ], sizeof( m_response_key ) );
									break;
								}
							}
							else
							{
								verboselog( 0, "bad crc\n" );
								m_read_buffer[ 0 ] = STATUS_ERROR;

								m_configuration_registers[ CONFIG_RC ]++;
								if ( m_configuration_registers[ CONFIG_RC ] >= m_configuration_registers[ CONFIG_RR ] )
								{
									// Too many bad reads, erase data
									std::fill( std::begin( m_data ), std::end( m_data ), 0 );
									std::fill( std::begin( m_data_key ), std::end( m_data_key ), 0 );
								}
							}

							verboselog( 1, "<- status: %02x\n", m_read_buffer[ 0 ] );

							verboselog( 1, "<- data: %02x%02x%02x%02x%02x%02x%02x%02x\n",
								m_read_buffer[ 2 ], m_read_buffer[ 3 ], m_read_buffer[ 4 ], m_read_buffer[ 5 ],
								m_read_buffer[ 6 ], m_read_buffer[ 7 ], m_read_buffer[ 8 ], m_read_buffer[ 9 ] );

							m_previous_byte = m_read_buffer[ 1 ];

							crc = calc_crc( m_read_buffer, 10 );
							m_read_buffer[ 10 ] = crc >> 8;
							m_read_buffer[ 11 ] = crc & 255;

							encrypt( m_read_buffer, m_read_buffer, sizeof( m_read_buffer ), m_response_key, 0xff );

							m_byte = 0;
							m_state = STATE_READ_DATA;
						}
						break;
					}

					m_bit = 0;
					m_shift = 0;
				}
			}
			break;

		case STATE_READ_DATA:
			// FIXME: Processing on the rising edge of the clock causes sda to change state while clock is high
			// which is not allowed.
			if( m_scl == 0 && state != 0 )
			{
				if( m_bit < 8 )
				{
					if( m_bit == 0 )
					{
						switch( m_state )
						{
						case STATE_READ_DATA:
							m_shift = m_read_buffer[ m_byte ];
							verboselog( 2, "<- read_buffer[ %d ]: %02x\n", m_byte, m_shift );
							break;
						}
					}

					m_sdar = ( m_shift >> 7 ) & 1;
					m_shift <<= 1;
					m_bit++;
				}
				else
				{
					m_bit = 0;
					m_sdar = 0;

					if( m_sdaw == 0 )
					{
						verboselog( 2, "ack <-\n" );
						m_byte++;

						if( m_byte == sizeof( m_read_buffer ) )
						{
							m_byte = 0;
							m_sdar = 1;
							m_state = STATE_LOAD_COMMAND;
						}
					}
					else
					{
						verboselog( 2, "nak <-\n" );
					}
				}
			}
			break;
		}
	}

	m_scl = state;
}

WRITE_LINE_MEMBER( zs01_device::write_sda )
{
	if( m_sdaw != state )
	{
		verboselog( 2, "sdaw=%d\n", state );
	}

	if( m_cs == 0 && m_scl != 0 )
	{
//      if( m_sdaw == 0 && state != 0 )
//      {
//          verboselog( 1, "goto stop\n" );
//          m_state = STATE_STOP;
//          m_sdar = 0;
//      }

		if( m_sdaw != 0 && state == 0 )
		{
			switch( m_state )
			{
			case STATE_STOP:
				verboselog( 1, "goto start\n" );
				m_state = STATE_LOAD_COMMAND;
				break;

//          default:
//              verboselog( 1, "skipped start (default)\n" );
//              break;
			}

			m_bit = 0;
			m_byte = 0;
			m_shift = 0;
			m_sdar = 0;
		}
	}

	m_sdaw = state;
}

READ_LINE_MEMBER( zs01_device::read_sda )
{
	if( m_cs != 0 )
	{
		verboselog( 2, "not selected\n" );
		return 1;
	}

	verboselog( 2, "sdar=%d\n", m_sdar );

	return m_sdar;
}

void zs01_device::nvram_default()
{
	m_response_to_reset[ 0 ] = 0x5a;
	m_response_to_reset[ 1 ] = 0x53;
	m_response_to_reset[ 2 ] = 0x00;
	m_response_to_reset[ 3 ] = 0x01;

	m_command_key[ 0 ] = 0xed;
	m_command_key[ 1 ] = 0x68;
	m_command_key[ 2 ] = 0x50;
	m_command_key[ 3 ] = 0x4b;
	m_command_key[ 4 ] = 0xc6;
	m_command_key[ 5 ] = 0x44;
	m_command_key[ 6 ] = 0x48;
	m_command_key[ 7 ] = 0x3e;

	memset( m_data_key, 0, sizeof( m_data_key ) );
	memset( m_configuration_registers, 0, sizeof( m_configuration_registers ) );
	memset( m_data, 0, sizeof( m_data ) );

	int expected_bytes = sizeof( m_response_to_reset ) + sizeof( m_command_key ) + sizeof( m_data_key ) + sizeof( m_configuration_registers ) + sizeof( m_data );

	if (!m_region.found())
	{
		logerror( "zs01(%s) region not found\n", tag() );
	}
	else if( m_region->bytes() != expected_bytes )
	{
		logerror( "zs01(%s) region length 0x%x expected 0x%x\n", tag(), m_region->bytes(), expected_bytes );
	}
	else
	{
		uint8_t *region = m_region->base();

		memcpy( m_response_to_reset, region, sizeof( m_response_to_reset ) ); region += sizeof( m_response_to_reset );
		memcpy( m_command_key, region, sizeof( m_command_key ) ); region += sizeof( m_command_key );
		memcpy( m_data_key, region, sizeof( m_data_key ) ); region += sizeof( m_data_key );
		memcpy( m_configuration_registers, region, sizeof( m_configuration_registers ) ); region += sizeof( m_configuration_registers );
		memcpy( m_data, region, sizeof( m_data ) ); region += sizeof( m_data );
	}
}

bool zs01_device::nvram_read( util::read_stream &file )
{
	std::size_t actual;
	bool result = !file.read( m_response_to_reset, sizeof( m_response_to_reset ), actual ) && actual == sizeof( m_response_to_reset );
	result = result && !file.read( m_command_key, sizeof( m_command_key ), actual ) && actual == sizeof( m_command_key );
	result = result && !file.read( m_data_key, sizeof( m_data_key ), actual ) && actual == sizeof( m_data_key );
	result = result && !file.read( m_configuration_registers, sizeof( m_configuration_registers ), actual ) && actual == sizeof( m_configuration_registers );
	result = result && !file.read( m_data, sizeof( m_data ), actual ) && actual == sizeof( m_data );
	return result;
}

bool zs01_device::nvram_write( util::write_stream &file )
{
	std::size_t actual;
	bool result = !file.write( m_response_to_reset, sizeof( m_response_to_reset ), actual ) && actual == sizeof( m_response_to_reset );
	result = result && !file.write( m_command_key, sizeof( m_command_key ), actual ) && actual == sizeof( m_command_key );
	result = result && !file.write( m_data_key, sizeof( m_data_key ), actual ) && actual == sizeof( m_data_key );
	result = result && !file.write( m_configuration_registers, sizeof( m_configuration_registers ), actual ) && actual == sizeof( m_configuration_registers );
	result = result && !file.write( m_data, sizeof( m_data ), actual ) && actual == sizeof( m_data );
	return result;
}
