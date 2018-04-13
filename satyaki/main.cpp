/*******************************************************************************
* Copyright (c) 2015-2017
* School of Electrical, Computer and Energy Engineering, Arizona State University
* PI: Prof. Shimeng Yu
* All rights reserved.
*
* This source code is part of NeuroSim - a device-circuit-algorithm framework to benchmark
* neuro-inspired architectures with synaptic devices(e.g., SRAM and emerging non-volatile memory).
* Copyright of the model is maintained by the developers, and the model is distributed under
* the terms of the Creative Commons Attribution-NonCommercial 4.0 International Public License
* http://creativecommons.org/licenses/by-nc/4.0/legalcode.
* The source code is free and you can redistribute and/or modify it
* by providing that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Developer list:
*   Pai-Yu Chen     Email: pchen72 at asu dot edu
*
*   Xiaochen Peng   Email: xpeng15 at asu dot edu
********************************************************************************/

#include <cstdio>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

#include "Cell.h"
#include "Array.h"
#include "formula.h"
#include "NeuroSim.h"
#include "Param.h"
#include "IO.h"
#include "Train.h"
#include "Test.h"
#include "Mapping.h"
#include "Definition.h"

using namespace std;

// the default values of the command line args
int nHIDE = 100;
bool OFFLINE = true, REAL = true;
double SIGMAdTOd = 0, MAXCOND = 5e-6, MINCOND = 100e-9, CONDLEVELS = 63, NONLINEAR = 2.4;
double ALPHA1 = 0.2, ALPHA2 = 0.1;

int main(int argc, char **argv)
{
    int c;

    while (1)
	{
		static struct option long_options[] =
		{
			{"neurons", required_argument, 0, 'n'},
			{"alpha1", required_argument, 0, 'a'},
			{"alpha2", required_argument, 0, 'b'},
			{"maxcond", required_argument, 0, 'm'},
			{"mincond", required_argument, 0, 'c'},
			{"condlevels", required_argument, 0, 'd'},
			{"sigmad", required_argument, 0, 's'},
			{"nonlinear", required_argument, 0, 'p'},
			{"online", no_argument, 0, 'o'},
            {"ideal", no_argument, 0, 'i'},
            {0, 0, 0, 0}
		};
		int option_index = 0;

		c = getopt_long_only(argc, argv, "n:a:b:m:c:d:s:p:", long_options, &option_index);

		if (c == -1)
			break;

		switch (c)
		{
			case 'n':
			  // printf("option -neurons with value `%s'\n", optarg);
			  nHIDE = std::atoi(optarg);
              break;

			case 'a':
			  // printf("option -alpha1 with value `%s'\n", optarg);
			  ALPHA1 = std::atof(optarg);
              break;

			case 'b':
			  // printf("option -alpha2 with value `%s'\n", optarg);
			  ALPHA2 = std::atof(optarg);
              break;

			case 'm':
			  // printf ("option -maxcond with value `%s'\n", optarg);
			  MAXCOND = std::atof(optarg);
              break;

			case 'c':
			  // printf ("option -mincond with value `%s'\n", optarg);
			  MINCOND = std::atof(optarg);
              break;

			case 'd':
			  // printf ("option -condlevels with value `%s'\n", optarg);
			  CONDLEVELS = std::atof(optarg);
              break;

			case 's':
			  // printf ("option -sigmad with value `%s'\n", optarg);
			  SIGMAdTOd = std::atof(optarg);
              break;

			case 'p':
			  // printf ("option -nonlinear with value `%s'\n", optarg);
			  NONLINEAR = std::atof(optarg);
              break;

            case 'o':
              // printf("option -online activated");
              OFFLINE = false;
              break;

            case 'i':
                REAL = false;
                break;

			default:
			  abort ();
		}
	}

    printf("\nnHIDE: %d, A1: %f, A2: %f, Min Cond: %f, Max Cond: %f, Cond
    levels: %f, Sigma: %f, Nonlinear: %f, Offline: %d, REAL:%d\n", nHIDE,
    ALPHA1, ALPHA2, MINCOND, MAXCOND, CONDLEVELS, SIGMAdTOd, NONLINEAR,
    OFFLINE, REAL);

	gen.seed(0);

	/* Restriction for black and white data only */
	if (param->numBitInput != 1) {
		puts("Error: this version of simulator only supports black and white input data");
		puts("Please use numBitInput=1 in Param.cpp");
		exit(-1);
	}

  auto prog_start = std::chrono::steady_clock::now();

	/* Load in MNIST data */
	ReadTrainingDataFromFile("patch60000_train.txt", "label60000_train.txt");
	ReadTestingDataFromFile("patch10000_test.txt", "label10000_test.txt");


    if (REAL)
	{
        /* Initialization of synaptic array from input to hidden layer */
	    arrayIH->Initialization<RealDevice>();

    	/* Initialization of synaptic array from hidden to output layer */
	    arrayHO->Initialization<RealDevice>();
    {

        /* Initialization of synaptic array from input to hidden layer */
	    arrayIH->Initialization<IdealDevice>();

    	/* Initialization of synaptic array from hidden to output layer */
	    arrayHO->Initialization<IdealDevice>();
   }
	/* Initialization of NeuroSim synaptic cores */
	param->relaxArrayCellWidth = 0;
	NeuroSimSubArrayInitialize(subArrayIH, arrayIH, inputParameterIH, techIH, cellIH);
	param->relaxArrayCellWidth = 1;
	NeuroSimSubArrayInitialize(subArrayHO, arrayHO, inputParameterHO, techHO, cellHO);
	/* Calculate synaptic core area */
	NeuroSimSubArrayArea(subArrayIH);
	NeuroSimSubArrayArea(subArrayHO);
	/* Calculate synaptic core standby leakage power */
	NeuroSimSubArrayLeakagePower(subArrayIH);
	NeuroSimSubArrayLeakagePower(subArrayHO);

	/* Initialize the neuron peripheries */
	NeuroSimNeuronInitialize(subArrayIH, inputParameterIH, techIH, cellIH, adderIH, muxIH, muxDecoderIH, dffIH);
	NeuroSimNeuronInitialize(subArrayHO, inputParameterHO, techHO, cellHO, adderHO, muxHO, muxDecoderHO, dffHO);
	/* Calculate the area and standby leakage power of neuron peripheries below subArrayIH */
	double heightNeuronIH, widthNeuronIH;
	NeuroSimNeuronArea(subArrayIH, adderIH, muxIH, muxDecoderIH, dffIH, &heightNeuronIH, &widthNeuronIH);
	double leakageNeuronIH = NeuroSimNeuronLeakagePower(subArrayIH, adderIH, muxIH, muxDecoderIH, dffIH);
	/* Calculate the area and standby leakage power of neuron peripheries below subArrayHO */
	double heightNeuronHO, widthNeuronHO;
	NeuroSimNeuronArea(subArrayHO, adderHO, muxHO, muxDecoderHO, dffHO, &heightNeuronHO, &widthNeuronHO);
	double leakageNeuronHO = NeuroSimNeuronLeakagePower(subArrayHO, adderHO, muxHO, muxDecoderHO, dffHO);

	/* Print the area of synaptic core and neuron peripheries */
	double totalSubArrayArea = subArrayIH->usedArea + subArrayHO->usedArea;
	double totalNeuronAreaIH = adderIH.area + muxIH.area + muxDecoderIH.area + dffIH.area;
	double totalNeuronAreaHO = adderHO.area + muxHO.area + muxDecoderHO.area + dffHO.area;
	printf("Total SubArray (synaptic core) area=%.4e m^2\n", totalSubArrayArea);
	printf("Total Neuron (neuron peripheries) area=%.4e m^2\n", totalNeuronAreaIH + totalNeuronAreaHO);
	printf("Total area=%.4e m^2\n", totalSubArrayArea + totalNeuronAreaIH + totalNeuronAreaHO);

	/* Print the standby leakage power of synaptic core and neuron peripheries */
	printf("Leakage power of subArrayIH is : %.4e W\n", subArrayIH->leakage);
	printf("Leakage power of subArrayHO is : %.4e W\n", subArrayHO->leakage);
	printf("Leakage power of NeuronIH is : %.4e W\n", leakageNeuronIH);
	printf("Leakage power of NeuronHO is : %.4e W\n", leakageNeuronHO);
	printf("Total leakage power of subArray is : %.4e W\n", subArrayIH->leakage + subArrayHO->leakage);
	printf("Total leakage power of Neuron is : %.4e W\n", leakageNeuronIH + leakageNeuronHO);

	/* Initialize weights and map weights to conductances for hardware implementation */
	WeightInitialize();
	if (param->useHardwareInTraining) { WeightToConductance(); }

	srand(0);	// Pseudorandom number seed
	for (int i=1; i<=param->totalNumEpochs/param->interNumEpochs; i++) {
    auto iter_start = std::chrono::steady_clock::now();
    Train(param->numTrainImagesPerEpoch, param->interNumEpochs);
		if (!param->useHardwareInTraining && param->useHardwareInTestingFF) { WeightToConductance(); }
		Validate();
	  auto iter_end = std::chrono::steady_clock::now();
		printf("Accuracy at %d epochs is : %.2f%\n", i*param->interNumEpochs, (double)correct/param->numMnistTestImages*100);
		/* Here the performance metrics of subArray also includes that of neuron peripheries (see Train.cpp and Test.cpp) */
		printf("\tRead latency=%.4e s\n", subArrayIH->readLatency + subArrayHO->readLatency);
		printf("\tWrite latency=%.4e s\n", subArrayIH->writeLatency + subArrayHO->writeLatency);
		printf("\tRead energy=%.4e J\n", arrayIH->readEnergy + subArrayIH->readDynamicEnergy + arrayHO->readEnergy + subArrayHO->readDynamicEnergy);
		printf("\tWrite energy=%.4e J\n", arrayIH->writeEnergy + subArrayIH->writeDynamicEnergy + arrayHO->writeEnergy + subArrayHO->writeDynamicEnergy);
    auto iter_duration = iter_end - iter_start;
    std::cout << "\tTime taken= " << std::chrono::duration<double> (iter_duration).count() << " sec" << std::endl;
    std::cout << std::endl;
  }

  auto prog_end = std::chrono::steady_clock::now();
  auto total_duration = prog_end - prog_start;
  std::cout << "Total simulation time: " << std::chrono::duration<double> (total_duration).count() << " sec" << std::endl;
	printf("\n");
	return 0;
}


