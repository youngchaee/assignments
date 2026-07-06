RISC-V 바이너리 코드 디코더 (RISC-V Binary Decoder)
1. 개요

    프로젝트 목적: RISC-V 아키텍처 규격을 기반으로 작성된 32비트 머신 코드(바이너리 파일)를 입력받아, 이를 사람이 읽을 수 있는 RISC-V 정수 명령어 세트(Assembly Representation)로 역어셈블(Disassemble)하는 프로그램 구현.

    대상 아키텍처: 32비트 RISC-V 가상 CPU 명세 (RV32I 규격 기반).

2. 주요 기능 및 디코딩 명세 (Instruction Specs)

바이너리 스트림에서 4바이트(32비트) 단위로 명령어를 읽어 들인 후, 각 필드의 비트 마스크 및 비트 연산을 통해 명령어 종류와 피연산자(Register, Immediate)를 추출함.

    지원 명령어 포맷 및 명령어 세트:

        R-Type: 레지스터 간 연산 수행 (add, sub, sll, slt, sltu, xor, srl, sra, or, and)

        I-Type: 레지스터와 즉시값(Immediate) 간 연산 및 로드 명령 (addi, slti, sltiu, xori, ori, andi, slli, srli, srai, lw)

        S-Type: 메모리 저장 명령 (sw)

        SB-Type: 조건 분기 명령 (beq, bne, blt, bge, bltu, bgeu)

        U-Type: 상위 비트 즉시값 적재 명령 (lui, auipc)

        UJ-Type: 무조건 점프 명령 (jal, jalr)

3. 구현 내용 및 핵심 로직 (Implementation Detail)

    바이너리 파일 파싱 (Stream Handling)

        std::ifstream 스트림을 이진 모드로 설정하여 입력받은 파일 내부의 RISC-V 머신 코드를 엔디안 규격에 맞춰 순차적으로 추출함.

    비트 필드 분석 (Field Extraction)

        32비트 명령어 데이터 구조에서 opcode, rd, func3, rs1, rs2, func7 필드를 분리하기 위해 std::bitset 인터페이스 및 문자열 슬라이싱 기법을 적용함.

    즉시값 복원 및 부호 확장 (Sign Extension)

        I, S, SB, U, UJ 포맷별로 흩어져 있는 즉시값(Immediate) 비트 조각들을 조합하는 함수(tosign)를 구현함.

        최상위 비트(MSB)를 기준으로 음수/양수 값을 정상 복원하는 정수형 부호 확장 알고리즘을 반영함.

    명령어 매칭 및 포맷 출력

        분석된 오프코드 및 하위 기능 필드(func3, func7) 조합 분기문을 구축하여 매칭되는 명령어 텍스트와 레지스터 번호, 즉시값 구조를 표준 출력(stdout) 형태로 정렬하여 출력함.
