# ------------------------------------------------------------------------------------------- #
# Imports section
import os
import sys
import subprocess
import argparse


GEN_PATH = os.path.join("nvs_flash",  "nvs_partition_generator", "nvs_partition_gen.py")
PAR_PATH = os.path.join("esptool_py", "esptool",                 "esptool.py")

IDF_COMP_PATH = os.path.expandvars(os.path.join("$IDF_PATH", "components"))
GEN_TOOL      = os.path.join(IDF_COMP_PATH, GEN_PATH)
PAR_TOOL      = os.path.join(IDF_COMP_PATH, PAR_PATH)
PAR_OFFSET    = 0x10000
INVOKE_ARGS_G = [sys.executable, GEN_TOOL, "--input"]
INVOKE_ARGS_F = [sys.executable, PAR_TOOL, "--port"]

# ------------------------------------------------------------------------------------------- #
# Function definitions
def GeneratePartition(inputFile):
    print("Generating data partition file...")
    output = inputFile.split('.')
    output = output[0] + '.bin'

    # Invokes the command
    #
    # nvs_partition_gen.py --input file.csv --output inputFile.bin --size 0x3000
    #
    invoke_args = INVOKE_ARGS_G + [inputFile, "--output", output, "--size", "0x3000"]
    subprocess.check_call(invoke_args)



def FlashPartition(args):
    print("Flashing to data partition...")

    # Invokes the command
    #
    # esptool.py --port /dev/ttyUSB0 write_flash 0x10000 part.bin
    #
    invoke_args = INVOKE_ARGS_F + [args.port, "write_flash", str(PAR_OFFSET), args.binary]
    subprocess.check_call(invoke_args)
    

# ------------------------------------------------------------------------------------------- #
# Main function
def main():
    global INVOKE_ARGS_G
    global INVOKE_ARGS_P

    parser = argparse.ArgumentParser('BNO055 Partition Tool. Generates binary' \
                                     ' partition data, and flashes to the device.')
    
    subparse = parser.add_subparsers(help='The two commands available are generate and' \
                                     ' flash. Type bnoPartition.py [command] -h for' \
                                     ' individual help.', dest='command')
    parseGen = subparse.add_parser('generate', help='Generates the binary file to flash.')
    parseGen.add_argument('csv', type=str, help='Enter the filepath to your CSV file.')

    parseFlash = subparse.add_parser('flash', help='Flashes the binary file to NVS.')
    parseFlash.add_argument('port', type=str, help='Enter the serial port in use.')
    parseFlash.add_argument('binary', type=str, help='Enter filepath to your binary data.')

    args = parser.parse_args()

    if args.command == 'generate':
        GeneratePartition(args.csv)
    elif args.command == 'flash':
        FlashPartition(args)

if __name__ == '__main__':
    main()

