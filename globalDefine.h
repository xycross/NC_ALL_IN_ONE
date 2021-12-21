#pragma once

#define SETBIT(ADDRESS,BIT)		(ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT)	(ADDRESS &= ~(1<<BIT))
#define TESTBIT(ADDRESS,BIT)	(ADDRESS & (1<<BIT))
#define TOGGLEBIT(ADDRESS, BIT) (ADDRESS ^= (1<<BIT))
#define SWAP(x, y)				(x)^=(y), (y)^=(x), (x)^=(y)

#define SET 	1
#define CLR 	0


#define	OP_DISTRIBUTION			0
#define	OP_SMUX					1
#define	OP_232_TO_OPTIC			2

#define	OP_OPT_BYPASS			3


#define MAXBUFFER					256

#define SMUX_LOOP_MAX				9					// S-MUX LOOP ���� ����
#define PLC_REP_PER_DET_MAX			50

#define DEFAULT_POLLING_DELAY_MS			10
#define DEFAULT_RESPONSE_DELAY				5
#define DEFAULT_OFFLINE_COUNT				1
#define DEFAULT_DETECTOR_255_OFFLINE_COUNT	5
#define DEFAULT_R_FIRE_CNT					1
#define DEFAULT_P_FIRE_CNT					1
#define DEFAULT_R_STATUS_CNT				1





#define MUX_CONFIG			8					// S-MUX Config,,
												// [0] �߰��/������ Polling ���� ����(ms)
												// [1] �߰��/������ �������� ���� (��ü ������ ����,, 2��� �ϸ� 2���� �����ð����� �������� ����)
												// [2] ������ ��������(255) ī��Ʈ ����Ʈ
												// [3] �߰��/������ offline ���� (���������� offline ���� ī��Ʈ�� �Ѿ���� Offline �����Ѵ�)
												// [4].0 S-MUX �����
												// [4].1 ��ü �ý��� ȸ�ν��� ON/OFF(�� S-MUX ���븸 ��)
												// [4].2 ��ü �ý��� ��� ON/OFF(�� S-MUX ���븸 ��)
												// [4].3 ��ü ������ LED ON/OFF(�� S-MUX ���븸 ��)
												// [4].4 �÷��� �޸� ����
												// [4].5 ��ȭ�� ī��Ʈ
												// [4].6 ����ȭ�� ī��Ʈ
												// [4].7 Repeater status ī��Ʈ
#define MUX_CONFIG_POLLING_DELAY				0
#define MUX_CONFIG_RESPONSE_DELAY				1
#define MUX_CONFIG_OFFLINE_COUNT				2
#define MUX_CONFIG_DETECTOR_255_OFFLINE_COUNT 	3
#define MUX_CONFIG_ALL_TEST_REP_DET				4
#define MUX_CONFIG_R_FIRE_CNT					5
#define MUX_CONFIG_P_FIRE_CNT					6
#define MUX_CONFIG_R_STATUS_CNT					7



#define MUX_CONFIG_RESET_BIT			0
#define MUX_CONFIG_ALL_CIRCUIT_TEST_BIT	1
#define MUX_CONFIG_ALL_OUT_BIT			2
#define MUX_CONFIG_ALL_DETECTOR_LED_BIT	3
#define MUX_CONFIG_FLASH_DEL_BIT		4
#define MUX_CONFIG_REPEATER_RESTORE_BIT	5

#define REPEATER_MAX		1000				// �߰��(word��)
#define REPEATER_INFO		8					// 	[0] command
												//	[1] status 	
												//	[2] �߰���� �����ѹ�
												//	[3] PLC �߰��� ������ ����
												//	[4] COM Err count
												//	[5] COM delay count {b0000 0000 1000 0000 - 7��° ��Ʈ�� set�Ǿ� ī��Ʈ ����}
												//  [6] ������ ù��° ��巹��
#define REPEATER_COMMAND			0
#define REPEATER_STATUS				1
#define REPEATER_LOOPNUM			2
#define REPEATER_DETECTOR_COUNT		3			// 7��° ��Ʈ�� plc�߰�� �Ǵ�,, ���� ��Ʈ 7���� ������ ����,,
#define REPEATER_COM_ERR			4
#define REPEATER_COM_DELAY_COUNT	5			// 7��° ��Ʈ�� timerī��Ʈ �÷���,, ���� ��Ʈ 7���� ī��Ʈ
#define DETECTOR_START_ADD			6
#define REPEATER_STATUS_CNT			7





#define PLC_DETECTOR_MAX	3000				// ������(char��)
#define PLC_DETECTOR_INFO	9					// 	[0] command
												//	[1] status/value 	
												//	[2] ����ȭ�簪
												//	[3] ��ȭ�簪
												//	[4] COM Err count
												//	[5] COM delay count {b1000 0000 - 7��° ��Ʈ�� set�Ǿ� ī��Ʈ ����}
												//  [6] ������ ���� 255�϶� Offline ī��Ʈ
												//	[7] ��ȭ�� ī��Ʈ
												//	[8] ����ȭ�� ī��Ʈ

#define DETECTOR_COMMAND				0
#define DETECTOR_STATUS					1
#define DETECTOR_P_FIRE					2
#define DETECTOR_R_FIRE					3
#define DETECTOR_COM_ERR				4
#define DETECTOR_COM_DELAY_COUNT		5
#define DETECTOR_COM_255_OFFLINE_COUNT	6
#define DETECTOR_R_FIRE_CNT				7
#define DETECTOR_P_FIRE_CNT				8



#define DETECTOR_CMD_PFIRE				0
#define DETECTOR_CMD_RFIRE				1
#define DETECTOR_CMD_LED				2
#define DETECTOR_CMD_S1T0				3
#define DETECTOR_CMD_RESTORE		4
#define DETECTOR_CMD_TEST				5
#define DETECTOR_CMD_OFFLINE			6
#define DETECTOR_CMD_COMMSTOP	7
#define DETECTOR_CMD_PLCOFF			15

//�߰�� ���� // �߰�� �Է� ������
#define REPEATER_INDATA_ADD_MIN_D00001		0	
#define REPEATER_INDATA_ADD_MAX_D01000		1000
#define FIND_REAL_REPEATER_INDATA_ADD		0

//�߰�� ��� //�߰�� ��� ������
#define REPEATER_OUTDATA_ADD_MIN_D01001		1001	
#define REPEATER_OUTDATA_ADD_MAX_D02000		2000
#define FIND_REAL_REPEATER_OUTDATA_ADD		1000

//������ ����ȭ�� ������
#define DETECTOR_P_FIRE_ADD_MIN_D10001		10001	
#define DETECTOR_P_FIRE_ADD_MAX_D15000		15000
#define FIND_REAL_DETECTOR_P_FIRE_ADD		10000

//������ ��ȭ�� ������
#define DETECTOR_R_FIRE_ADD_MIN_D15001		15001	
#define DETECTOR_R_FIRE_ADD_MAX_D20000		20000
#define FIND_REAL_DETECTOR_R_FIRE_ADD		15000

//������ ��
#define DETECTOR_VALUE_ADD_MIN_D20001		20001	
#define DETECTOR_VALUE_ADD_MAX_D25000		25000
#define FIND_REAL_DETECTOR_VALUE_ADD		20000

//������ ����/���� ��
#define DETECTOR_COMMAND_ADD_MIN_D25001		25001	
#define DETECTOR_COMMAND_ADD_MAX_D30000		30000
#define FIND_REAL_DETECTOR_COMMAND_ADD		25000

// �߰���� ���� ��ȣ (��� SMUX�� BIND �Ǿ� �ִ°�)
#define REPEATER_LOOP_ADD_MIN_D5001			5001	
#define REPEATER_LOOP_ADD_MAX_D6000			6000
#define FIND_REAL_REPEATER_LOOP_ADD			5000

// ������ ����
#define REP_PER_DEC_COUNT_ADD_MIN_D6001		6001	
#define REP_PER_DEC_COUNT_ADD_MAX_D7000		7000
#define FIND_REAL_REP_PER_DEC_COUNT_ADD		6000

// �������� ù��° ��巹��
#define DETECTOR_FIRST_ADD_MIN				7001	
#define DETECTOR_FIRST_ADD_MAX				8000
#define FIND_REAL_DETECTOR_FIRST_ADD		7000

// S-MUX CONFIG

#define SMUX_CONFIG_POLLING_DELAY_ADD				9001
#define SMUX_CONFIG_RESPONSE_DELAY_ADD				9002
#define SMUX_CONFIG_OFFLINE_COUNT_ADD				9003
#define SMUX_CONFIG_DETECTOR_255_OFFLINE_COUNT_ADD 	9004
#define SMUX_CONFIG_ALL_TEST_REP_DET_ADD			9005
#define SMUX_CONFIG_R_FIRE_CNT_ADD					9006
#define SMUX_CONFIG_P_FIRE_CNT_ADD					9007
#define SMUX_CONFIG_R_STATUS_CNT_ADD				9008







