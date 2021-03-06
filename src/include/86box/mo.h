/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Implementation of a generic Magneto-Optical Disk drive
 *		commands, for both ATAPI and SCSI usage.
 *
 *
 *
 * Authors:	Natalia Portillo <claunia@claunia.com>
 *          Fred N. van Kempen, <decwiz@yahoo.com>
 *		    Miran Grca, <mgrca8@gmail.com>
 *
 *		Copyright 2020 Miran Grca.
 */
#ifndef EMU_MO_H
#define EMU_MO_H


#define MO_NUM			  4

#define BUF_SIZE 32768

#define MO_TIME	10.0


typedef struct {
    uint32_t	sectors;
    uint16_t	bytes_per_sector;
    int64_t	disk_size;
    char	name[255];
} mo_type_t;

#define KNOWN_MO_TYPES 10
static const mo_type_t mo_types[KNOWN_MO_TYPES] = {
    // 3.5" standard M.O. disks
    {	248826,  512, 127398912, "3.5\" 128Mb M.O. (ISO 10090)" },
    {	446325,  512, 228518400, "3.5\" 230Mb M.O. (ISO 13963)" },
    {	1041500,  512, 533248000, "3.5\" 540Mb M.O. (ISO 15498)" },
    {	310352,  2048, 635600896, "3.5\" 640Mb M.O. (ISO 15498)" },
    {	605846,  2048, 1240772608, "3.5\" 1.3Gb M.O. (GigaMO)" },
    {	1063146,  2048, 2177323008, "3.5\" 2.3Gb M.O. (GigaMO 2)" },
    // 5.25" M.O. disks
    {573624, 512, 293695488, "5.25\" 600Mb M.O."},
    {314568, 1024, 322117632, "5.25\" 650Mb M.O."},
    {904995, 512, 463357440, "5.25\" 1Gb M.O."},
    {637041, 1024, 652329984, "5.25\" 1.3Gb M.O."},
};

typedef struct
{
    char vendor[8];
    char model[16];
    char revision[4];
    int8_t supported_media[KNOWN_MO_TYPES];
} mo_drive_type_t;

static const mo_drive_type_t mo_drive_types[22] = {
    {"86BOX", "MAGNETO OPTICAL", "1.00",{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
    {"FUJITSU", "M2512A", "1314",{1, 1, 0, 0, 0, 0, 0, 0, 0}},
    {"FUJITSU", "M2513-MCC3064SS", "1.00",{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}},
    {"FUJITSU", "MCE3130SS", "0070",{1, 1, 1, 1, 1, 0, 0, 0, 0, 0}},
    {"FUJITSU", "MCF3064SS", "0030",{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}},
    {"FUJITSU", "MCJ3230UB-S", "0040",{1, 1, 1, 1, 1, 1, 0, 0, 0, 0}},
    {"HP", "S6300.65", "1.00",{0, 0, 0, 0, 0, 0, 1, 1, 0, 0}},
    {"HP", "C1716C", "1.00",{0, 0, 0, 0, 0, 0, 1, 1, 0, 1}},
    {"IBM", "0632AAA", "1.00",{0, 0, 0, 0, 0, 0, 1, 1, 0, 0}},
    {"IBM", "0632CHC", "1.00",{0, 0, 0, 0, 0, 0, 1, 1, 0, 1}},
    {"IBM", "0632CHX", "1.00",{0, 0, 0, 0, 0, 0, 1, 1, 0, 1}},
    {"IBM", "MD3125A", "1.00",{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"IBM", "MD3125B", "1.00",{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"IBM", "MTA-3127", "1.00",{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"IBM", "MTA-3230", "1.00",{1, 1, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"MATSHITA", "LF-3000", "1.00",{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"MOST", "RMD-5100", "1.00",{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"RICOH", "RO-5031E", "1.00",{0, 0, 0, 0, 0, 0, 1, 1, 0, 0}},
    {"SONY", "SMO-C301", "1.00",{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"SONY", "SMO-C501", "1.00",{0, 0, 0, 0, 0, 0, 1, 1, 0, 0}},
    {"TEAC", "OD-3000", "1.00",{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"TOSHIBA", "OD-D300", "1.00",{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
};

enum {
    MO_BUS_DISABLED = 0,
    MO_BUS_ATAPI = 4,
    MO_BUS_SCSI,
    MO_BUS_USB
};

typedef struct {
    uint8_t 	id,
		res, res0,		/* Reserved for other ID's. */
		res1,
		ide_channel, scsi_device_id,
		bus_type,		/* 0 = ATAPI, 1 = SCSI */
		bus_mode,		/* Bit 0 = PIO suported;
					   Bit 1 = DMA supportd. */
		read_only,		/* Struct variable reserved for
					   media status. */
		pad, pad0;

    uint32_t	medium_size,
		base;
    uint16_t 	sector_size;
    uint8_t 	type;

    void	*priv;

    FILE	*f;

    wchar_t	image_path[1024],
		prev_image_path[1024];
} mo_drive_t;

typedef struct {
    uint8_t	id,
		error, status,
		phase,
		features,
		is_dma,
		do_page_save,
		unit_attention;

    mo_drive_t	*drv;

    uint16_t	request_length,
		max_transfer_len;

    int		requested_blocks, packet_status,
		request_pos, old_len,
		total_length;

    uint32_t	sector_pos, sector_len,
		packet_len, pos,
		seek_pos;

    double	callback;

    mode_sense_pages_t ms_pages_saved;

    uint8_t	*buffer,
		atapi_cdb[16],
		current_cdb[16],
		sense[256];
} mo_t;


extern mo_t		*mo[MO_NUM];
extern mo_drive_t	mo_drives[MO_NUM];
extern uint8_t		atapi_mo_drives[8];
extern uint8_t		scsi_mo_drives[16];

#define mo_sense_error dev->sense[0]
#define mo_sense_key dev->sense[2]
#define mo_asc dev->sense[12]
#define mo_ascq dev->sense[13]


#ifdef __cplusplus
extern "C" {
#endif

extern void     mo_disk_close(mo_t *dev);
extern void     mo_disk_reload(mo_t *dev);
extern void	mo_insert(mo_t *dev);

extern void	mo_global_init(void);
extern void	mo_hard_reset(void);

extern void	mo_reset(scsi_common_t *sc);
extern int	mo_load(mo_t *dev, wchar_t *fn);
extern void	mo_close();

#ifdef __cplusplus
}
#endif


#endif	/*EMU_MO_H*/
