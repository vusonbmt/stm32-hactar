// Copyright 2011 Christoph Reiter
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//

#include <stdio.h>
#include <string.h>

#include "stm32f10x_rcc.h"

#include <hactar/term.h>
#include <hactar/misc.h>

static void printClockStatus(char **args)
{
    RCC_ClocksTypeDef test;
    RCC_GetClocksFreq(&test);

    iprintf("ADC   %u\n", (unsigned int)test.ADCCLK_Frequency);
    iprintf("HCLK  %u\n", (unsigned int)test.HCLK_Frequency);
    iprintf("PCLK1 %u\n", (unsigned int)test.PCLK1_Frequency);
    iprintf("PCLK2 %u\n", (unsigned int)test.PCLK2_Frequency);
    iprintf("SYS   %u\n", (unsigned int)test.SYSCLK_Frequency);
}

static void printMemoryStatus(char **args)
{
    MemoryInfo info = getMemoryInfo();

    iprintf("\nFlash:  %5u %% free\n", info.flash_free_);
    iprintf(" Text   %5u Bytes\n", info.text_);
    iprintf(" ROData %5u Bytes\n", info.ro_data_);

    iprintf("\nRAM:    %5u %% free\n", info.ram_free_);
    iprintf(" Data   %5u Bytes\n", info.data_);
    iprintf(" BSS    %5u Bytes\n", info.bss_);
    iprintf(" Heap   %5u Bytes\n", info.heap_);
    iprintf(" Malloc %5u Bytes\n", info.malloc_);
}

static void getCommand(char *dest, char **args, size_t count, size_t arg_count)
{
    char* start_arg = dest;
    size_t current_arg = 0;

    while(1)
    {
        *dest = getchar();

        if(*dest == '\n' || *dest == '\r' || *dest == ' ' || *dest == EOF)
        {
            if(current_arg < (arg_count - 1) && start_arg <= dest)
            {
                args[current_arg++] = start_arg;
                start_arg = dest + 1;
            }

            if(*dest != ' ')
            {
                args[current_arg] = NULL;
                *dest = '\0';
                break;
            }

            *dest = '\0';
        }

        if(count == 0)
            continue;

        count--;
        dest++;
    }
}

void startTerminal(TermCommand* user_cmds, size_t num_user_cmds)
{
    static const TermCommand cmds[] = {
            {
                    .command_ = "clk",
                    .description_ = "Clock information",
                    .function_ = &printClockStatus,
            },
            {
                    .command_ = "mem",
                    .description_ = "Memory status",
                    .function_ = &printMemoryStatus,
            },
            {
                    .command_ = "help",
                    .description_ = "Command list",
                    .function_ = NULL,
            },
            {
                    .command_ = "exit",
                    .description_ = "Exit terminal",
                    .function_ = NULL,
            },
    };

    char input[HACTAR_TERM_INPUT_BUFFER_SIZE];
    char *args[HACTAR_TERM_INPUT_ARG_COUNT];
    size_t i, num_cmds = sizeof(cmds) / sizeof(cmds[0]);

    while(1)
    {
        fflush(stdout);
        fiprintf(stderr, "> ");
        fflush(stderr);

        getCommand(input, args,
                  HACTAR_TERM_INPUT_BUFFER_SIZE,
                  HACTAR_TERM_INPUT_ARG_COUNT);

        if(strcmp(args[0], "exit") == 0)
            break;

        if(strcmp(args[0], "help") == 0)
        {
            char *help_format = "%-5s: %s\n";
            for(i = 0; i < num_cmds; i++)
            {
                iprintf(help_format, cmds[i].command_, cmds[i].description_);
            }

            if(user_cmds == NULL)
                continue;

            iprintf("--------------------------\n");

            for(i = 0; i < num_user_cmds; i++)
            {
                iprintf(help_format, user_cmds[i].command_,
                        user_cmds[i].description_);
            }

            continue;
        }

        uint32_t found = 0;

        // User commands first so new std commands don't override them
        for(i = 0; i < num_user_cmds && user_cmds != NULL; i++)
        {
            if(strcmp(user_cmds[i].command_, args[0]) == 0)
            {
                user_cmds[i].function_(&args[1]);
                found = 1;
                break;
            }
        }

        if(found)
            continue;

        // Now the builtin ones
        for(i = 0; i < num_cmds; i++)
        {
            if(strcmp(cmds[i].command_, args[0]) == 0)
            {
                cmds[i].function_(&args[1]);
                found = 1;
                break;
            }
        }

        if(!found)
        {
            fflush(stdout);
            fiprintf(stderr, "Unknown command\n");
        }
    }
}