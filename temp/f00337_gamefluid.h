// f00337_gamefluid.h
//

#include "f00337_gamefluid.e"
#define LZZ_INLINE inline
GameFluid::GameFluid ()
                    {
		
		hasRead = false;
		
		UNIT_MIN = FLUID_UNIT_MIN;
		UNIT_MAX = FLUID_UNIT_MAX;
		
		invalidated = true;
		
		F_UNIT_MIN = UNIT_MIN;
		F_UNIT_MAX = UNIT_MAX;
		
		watchMinX = -1;
		watchMaxX = -1;
		watchMinY = -1;
		watchMaxY = -1;
		watchMinZ = -1;
		watchMaxZ = -1;
		
	}
void GameFluid::init (Singleton * _singleton)
                                         {
		singleton = _singleton;
		volSizePrim = singleton->volSizePrim;
		bufAmount = 1;
		volSizePrimBuf = volSizePrim+bufAmount*2;
		
		curTick = 0;
		maxTicks = 1;//volSizePrim/8;
		
		vspMin = bufAmount;
		vspMax = volSizePrimBuf-bufAmount;
		
		fluidData = new int[volSizePrimBuf*volSizePrimBuf*volSizePrimBuf*4];
		extraData = new int[volSizePrimBuf*volSizePrimBuf*volSizePrimBuf*4];
		
		totSize = volSizePrimBuf*volSizePrimBuf*volSizePrimBuf;
		
	}
void GameFluid::writeFluidData ()
                              {
		int i;
		int j;
		int k;
		
		float fi;
		float fj;
		float fk;
		
		int ind;
		
		
		
		if (hasRead) {
			
			
			for (k = vspMin; k < vspMax; k++) {
				fk = k + (singleton->volMinInPixels[2]);
				for (j = vspMin; j < vspMax; j++) {
					fj = j + (singleton->volMinInPixels[1]);
					for (i = vspMin; i < vspMax; i++) {
						fi = i + (singleton->volMinInPixels[0]);
						
						ind = (i + j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf)*4;
						//indSrc = ((i-bufAmount) + (j-bufAmount)*volSizePrim + (k-bufAmount)*volSizePrim*volSizePrim)*4;
												
						singleton->gw->setCellAtCoords(
							(i-bufAmount)+singleton->volMinInPixels.getIX(),
							(j-bufAmount)+singleton->volMinInPixels.getIY(),
							(k-bufAmount)+singleton->volMinInPixels.getIZ(),
							ind,
							fluidData,
							extraData
							
							// fluidData[ind+0],
							// fluidData[ind+1],
							// fluidData[ind+2],//lastFluidData[ind+1],
							// fluidData[ind+3]
						);
						
						
					}
					
					
				}	
			}
			
			
		}
	}
void GameFluid::readFluidData ()
                             {
		hasRead = true;
		
		
		int i;
		int j;
		int k;
		int q;
		
		float fi;
		float fj;
		float fk;
		
		int ind;
		int ind2;
		
		
		for (i = 0; i < totSize; i++) {
			ind = i*4;
			
			for (q = 0; q < 4; q++) {
				fluidData[ind+q] = UNIT_MIN;
				extraData[ind+q] = UNIT_MIN;
			}
			
			fluidData[ind+E_PTT_TER] = UNIT_MAX;
			
		}
		
		
		for (k = vspMin; k < vspMax; k++) {
			fk = k + (singleton->volMinInPixels[2]);
			for (j = vspMin; j < vspMax; j++) {
				fj = j + (singleton->volMinInPixels[1]);
				for (i = vspMin; i < vspMax; i++) {
					fi = i + (singleton->volMinInPixels[0]);
					
					ind = (i + j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf)*4;
					//indSrc = ((i-bufAmount) + (j-bufAmount)*volSizePrim + (k-bufAmount)*volSizePrim*volSizePrim)*4;
					
					singleton->gw->getCellAtCoords(
						(i-bufAmount)+singleton->volMinInPixels.getIX(),
						(j-bufAmount)+singleton->volMinInPixels.getIY(),
						(k-bufAmount)+singleton->volMinInPixels.getIZ()
					);
					
					for (q = 0; q < 4; q++) {
						ind2 = ind+q;
						fluidData[ind2] = singleton->gw->tempCellData[q];
						extraData[ind2] = singleton->gw->tempCellData2[q];
					}
					//lastFluidData[ind+E_PTT_WAT] = extraData[ind+E_PTT_IDE];
					
				}
				
				
			}
		}
		
	}
void GameFluid::applyMods ()
                         {
		while (modStack.size() > 0) {
			applyUnitModification(
				&(modStack.back().basePos),
				modStack.back().brushAction,
				modStack.back().modType,
				modStack.back().radius
			);
			modStack.pop_back();
		}
	}
void GameFluid::updateFluidData ()
                               {
		
		int i;
		int j;
		int k;
		int c;
		int dir;
		int n;
		int p;
		int q;
		
		int tempv;
		
		int testInd;
		int testInd2;
		int testInd3;
		int testI;
		int testJ;
		int testK;
		
		int numCells;
		
		int ind;
		int ind2;
		
		int totWat;
		
		float fVSP = volSizePrimBuf;
		
		bool doProc;
		
		float fi;
		float fj;
		float fk;
		
		int ox;
		int oy;
		int oz;
		
		int curId;
		int tempi;
		
		int* bldVal;
		int* terVal;
		int* watVal;
		int* watVal2;
		int* watVal3;
		int* terVal2;
		int* bldVal2;
		int* ideVal;
		int* edgVal;
		
		int curCollectedWater;
		
		int fluidBodyCount;
		
		
		// k is largest at the top
		
		bool bTouchesAir;
		
		// float fSimp[4];
		// int iSimp[4];
		
		float disFromTop;
		float disFromBot;
		float spanZ;
		float minZ;
		
		float maxDif = 1.0f/4.0f;// /8.0f;
		
		float zv;
		
		bool isAir;
		bool isEmptyWater;
		bool didPrint = false;
		
		bool isInBounds;
		
		int minV0 = min( ((curTick)*totSize)/maxTicks, totSize-1 );
		int minV1 = min( ((curTick+1)*totSize)/maxTicks, totSize-1 );
		
		
		if (hasRead) {
			
		}
		else {
			return;
		}
		
		
		//if (curTick == 0) {
			// clear water ids and edges
			for (i = 0; i < totSize; i++) {
				ind = i;
				extraData[ind*4+E_PTT_IDE] = 0;
				extraData[ind*4+E_PTT_STB] = 0;
				fluidData[ind*4+E_PTT_LST] = fluidData[ind*4+E_PTT_WAT];
				//lastFluidData[ind*4+E_PTT_WAT] = fluidData[ind*4+E_PTT_WAT];
			}
			
			for (i = 0; i < fsVec.size(); i++) {
				fsVec[i].fluidIds.clear();
				fsVec[i].idealCellIds.clear();
			}
			fsVec.clear();
			
			// for (i = 0; i < fsPlaneVec.size(); i++) {
			// 	fsPlaneVec[i].fluidIds.clear();
			// }
			// fsPlaneVec.clear();
			
			
			
		//}
		
		
		
		
		
		// find stable regions
		fluidBodyCount = 1;
		for (i = minV0; i <= minV1; i++) {
			ind = i;
			
			
			watVal = &(fluidData[ind*4+E_PTT_WAT]);
			ideVal = &(extraData[ind*4+E_PTT_STB]);
			
			if (
				(*watVal >= 0) &&
				(*ideVal == 0)
			) {
				
				if (findStableRegions(ind,fluidBodyCount)) {
					fluidBodyCount++;
				}
			}
		}
		
		
		
		// flood fill water ids
		fluidBodyCount = 1;
		for (i = minV0; i <= minV1; i++) {
			ind = i;
			
			watVal = &(fluidData[ind*4+E_PTT_WAT]);
			ideVal = &(extraData[ind*4+E_PTT_IDE]);
			
			if (
				(*watVal >= 0) &&
				(*ideVal == 0)
			) {
				
				if (floodFillId(ind,fluidBodyCount)) {
					fluidBodyCount++;
				}
				
			}
			
		}
		
		
		
		
		
		
		// find most ideal areas to distribute water
		for (n = 0; n < fsVec.size(); n++) {
			
			#ifdef DEBUG_BOUNDS
			isInBounds = false;
			#endif
			
			for (p = 0; p < fsVec[n].fluidIds.size(); p++) {
				ind = fsVec[n].fluidIds[p];
				
				k = ind/(volSizePrimBuf*volSizePrimBuf);
				j = (ind - k*volSizePrimBuf*volSizePrimBuf)/volSizePrimBuf;
				i = ind - ( j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf );
				
				#ifdef DEBUG_BOUNDS
				isInBounds = isInBounds||inBounds(i,j,k);
				#endif
				
				
				bldVal = &(extraData[ind*4+E_PTT_BLD]);
				terVal = &(fluidData[ind*4+E_PTT_TER]);
				watVal = &(fluidData[ind*4+E_PTT_WAT]);
				
				isAir = 
					(*watVal == UNIT_MIN) &&
					(*bldVal == UNIT_MIN) &&
					(*terVal == UNIT_MIN);
				
				isEmptyWater = (*watVal == 0);
				
				#ifdef DEBUG_BOUNDS
				if (isInBounds) {
					if (isAir) {cout << "isAir\n";}
					cout << "watVal " << *watVal << "\n";
				}
				#endif
				
				
				if (isAir) {
					// check for water above
					testInd = ind + (volSizePrimBuf*volSizePrimBuf);
					watVal2 = &(fluidData[testInd*4+E_PTT_WAT]);
					
					if (*watVal2 > 0) {
						fsVec[n].idealCellIds.push_back(ind);
					}
					else {
						for (dir = 0; dir < 4; dir++) {
							testI = (i+DIR_VECS_I[dir][0]);
							testJ = (j+DIR_VECS_I[dir][1]);
							testK = (k+DIR_VECS_I[dir][2]);
							testInd2 = 
								testI +
								testJ*volSizePrimBuf +
								testK*volSizePrimBuf*volSizePrimBuf;
							
							watVal3 = &(fluidData[testInd2*4+E_PTT_WAT]);
							
							// has (partially) full water cell to side
							if (*watVal3 >= 0) {
								
								// if unit below that one is earth
								testInd3 = testInd2 - volSizePrimBuf*volSizePrimBuf;
								if (
									(fluidData[testInd3*4+E_PTT_TER] > UNIT_MIN) ||
									(extraData[testInd3*4+E_PTT_BLD] > UNIT_MIN)
									// || (fluidData[testInd3*4+E_PTT_WAT] == UNIT_MIN)
								) {
									fsVec[n].idealCellIds.push_back(ind);
									break;
								}
							}
						}
					}
				}
			}
			
			#ifdef DEBUG_BOUNDS
			if (isInBounds) {
				didPrint = true;
				cout << "cells considered " << fsVec[n].fluidIds.size() << "\n";
				cout << "cells pushed " << fsVec[n].idealCellIds.size() << "\n";
			}
			#endif
			
			
		}
		
		
		
		
	
		// remove water from top of fluid body
		for (n = 0; n < fsVec.size(); n++) {
			
			
			fsVec[n].collectedWater = 0;
			curCollectedWater = 0;
			
			minZ = fsVec[n].minZ;
			spanZ = fsVec[n].maxZ - fsVec[n].minZ;
			
			#ifdef DEBUG_BOUNDS
			isInBounds = false;
			#endif
			
			totWat = 0;
			
			for (p = 0; p < fsVec[n].fluidIds.size(); p++) {
				ind = fsVec[n].fluidIds[p];
				
				k = ind/(volSizePrimBuf*volSizePrimBuf);
				j = (ind - k*volSizePrimBuf*volSizePrimBuf)/volSizePrimBuf;
				i = ind - ( j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf );
				
				#ifdef DEBUG_BOUNDS
				isInBounds = isInBounds||inBounds(i,j,k);
				#endif
				
				
				
				
				
				fk = k;
				disFromBot = (fk-minZ)/(spanZ);
				disFromTop = 1.0f - disFromBot;
				
				disFromBot = mixf(0.25,0.75,disFromBot);
				disFromTop = mixf(0.25,0.75,disFromTop);
				
				if (spanZ <= 3.0f) {
					disFromBot = 1.0f;
				}
				
				
				//terVal = &(fluidData[ind*4+E_PTT_TER]);
				watVal = &(fluidData[ind*4+E_PTT_WAT]);
				//ideVal = &(extraData[ind*4+E_PTT_IDE]);
				
				totWat += max(*watVal,0);
				
				// make sure no water is above or that there is earth above
				testInd = ind + (volSizePrimBuf*volSizePrimBuf);
				watVal2 = &(fluidData[testInd*4+E_PTT_WAT]);
				terVal2 = &(fluidData[testInd*4+E_PTT_TER]);
				bldVal2 = &(extraData[testInd*4+E_PTT_BLD]);
				
				if (
					(*watVal > 0) &&
					(
						(*watVal2 <= 0) ||
						(*terVal2 != UNIT_MIN) ||
						(*bldVal2 != UNIT_MIN)
					)
				) {
					curCollectedWater = 
						min(
							((int)(disFromBot*F_UNIT_MAX*maxDif + 1.0f)),
							*watVal
						);
					*watVal -= curCollectedWater;
					fsVec[n].collectedWater += curCollectedWater;
				}
			}
			
			
			
			// cout << "fsVec[n].collectedWater " << fsVec[n].collectedWater << "\n";
			
			// if (fsVec[n].collectedWater == 0) {
			// 	cout << "fsVec[n].fluidIds.size() " << fsVec[n].fluidIds.size() << "\n";
			// }
			
			
			
			#ifdef DEBUG_BOUNDS
			if (isInBounds) {
				didPrint = true;
				cout << "totWat " << totWat << "\n";
				cout << "fsVec[n].collectedWater " << fsVec[n].collectedWater << "\n";
			}
			#endif
			
		}
		
		
		
		
		
		// add water to ideal spots
		for (n = 0; n < fsVec.size(); n++) {
			
			numCells = fsVec[n].idealCellIds.size();
			
			if (numCells > 0) {
				
				for (p = 0; p < numCells; p++) {
					ind = fsVec[n].idealCellIds[p];
					watVal = &(fluidData[ind*4+E_PTT_WAT]);
					
					if (*watVal == UNIT_MIN) {
						*watVal = 0;
					}
				}
				
				
				
				if ( (fsVec[n].collectedWater/numCells) > UNIT_MAX ) {
					curCollectedWater = UNIT_MAX;
				}
				else {
					curCollectedWater = fsVec[n].collectedWater/numCells;
				}
				
				//cout << "curCollectedWater " << curCollectedWater << "\n";
				
				curCollectedWater = curCollectedWater*maxDif;
				
				#ifdef DEBUG_BOUNDS
				isInBounds = false;
				#endif
				
				
				for (p = 0; p < numCells; p++) {
					ind = fsVec[n].idealCellIds[p];
					
					k = ind/(volSizePrimBuf*volSizePrimBuf);
					j = (ind - k*volSizePrimBuf*volSizePrimBuf)/volSizePrimBuf;
					i = ind - ( j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf );
					
					#ifdef DEBUG_BOUNDS
					isInBounds = isInBounds||inBounds(i,j,k);
					#endif
					
					
					watVal = &(fluidData[ind*4+E_PTT_WAT]);
					
					
					if (curCollectedWater > 0) {
						
						tempi = min(UNIT_MAX - *watVal,curCollectedWater);
						
						*watVal += tempi;
						fsVec[n].collectedWater -= tempi;
					}
					
					
					
					
					
					
					if (fsVec[n].collectedWater <= 0) {
						break;
					}
					
				}
				
				#ifdef DEBUG_BOUNDS
				if (isInBounds) {
					didPrint = true;
					cout << "dist water " << fsVec[n].collectedWater << "\n\n";
				}
				#endif
				
			}
		}
		
		
		// add water to bottom of fluid body
		for (n = 0; n < fsVec.size(); n++) {
			
			curCollectedWater = 0;
			minZ = fsVec[n].minZ;
			
			
			
			
			while (fsVec[n].collectedWater > 0) {
				
				
				for (q = fsVec[n].minZ; q <= fsVec[n].maxZ; q++) {
					for (p = 0; p < fsVec[n].fluidIds.size(); p++) {
						ind = fsVec[n].fluidIds[p];
						
						k = ind/(volSizePrimBuf*volSizePrimBuf);
						watVal = &(fluidData[ind*4+E_PTT_WAT]);
						
						if (k == q) {
							
							if (fsVec[n].collectedWater > UNIT_MAX) {
								tempv = UNIT_MAX;
							}
							else {
								tempv = fsVec[n].collectedWater;
							}
							
							//if (fluidData[ind*4+E_PTT_TER] == UNIT_MIN) {
								
								curCollectedWater = min(
									((int)((UNIT_MAX - max(*watVal,0) )*maxDif + 1.0f )),
									tempv
								);
								
								if (curCollectedWater > 0) {
									if (*watVal == UNIT_MIN) {
										*watVal = 0;
									}
									
									tempi = min(UNIT_MAX - *watVal,curCollectedWater);
									
									*watVal += tempi;
									fsVec[n].collectedWater -= tempi;
								}
								
							//}
						}
						
						
						// out of water, break both parent loops
						if (fsVec[n].collectedWater <= 0) {
							p = fsVec[n].fluidIds.size() + 1;
							q = fsVec[n].maxZ + 1;
						}
					}
				}
				
				
			}
		}
	
		
		if (didPrint) {
			watchMinX = -1;
		}
		
		
		curTick++;
		if (curTick == maxTicks) {
			curTick = 0;
		}
		
		
		
	}
bool GameFluid::findStableRegions (int startInd, int newId)
                                                        {
		indexStack.clear();
		indexStack.push_back(startInd);
		
		int dir;
		
		int ind;
		int testInd;
		int testInd2;
		int* bldVal;
		int* bldVal2;
		int* terVal;
		int* terVal2;
		int* watVal;
		int* watVal2;
		int* ideVal;
		
		int i;
		int j;
		int k;
		int n;
		
		int* curVec;
		
		int foundInd;
		
		int testI;
		int testJ;
		int testK;
		
		bool notFound;
		bool isAir;
		bool terBelow;
		
		int watCount;
		int maxSize;
		
		long long int totWat;
		long long int totWat2;
		long long int divWat;
		
		//fsPlaneVec.push_back(FluidPlane());
		
		
		FluidPlane* fsPtr = &fluidPlane;//&(fsPlaneVec.back());
		fsPtr->fluidIds.clear();
		
		int emptyWaterCount = 0;
		int airCount = 0;
		int targWat;
		
		while (indexStack.size() > 0) {
			
			ind = indexStack.back();
			extraData[ind*4+E_PTT_STB] = newId;
			fsPtr->fluidIds.push_back(ind);
			
			if (fluidData[ind*4+E_PTT_WAT] == 0) {
				emptyWaterCount++;
			}
			
			
			k = ind/(volSizePrimBuf*volSizePrimBuf);
			j = (ind - k*volSizePrimBuf*volSizePrimBuf)/volSizePrimBuf;
			i = ind - ( j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf );
						
			
			foundInd = -1;
			
			for (dir = 0; dir < 4; dir++) {
				testI = (i+DIR_VECS_I[dir][0]);
				testJ = (j+DIR_VECS_I[dir][1]);
				testK = (k+DIR_VECS_I[dir][2]);
				testInd = 
					testI +
					testJ*volSizePrimBuf +
					testK*volSizePrimBuf*volSizePrimBuf;
				
				testInd2 = testInd - volSizePrimBuf*volSizePrimBuf;
				
				terVal = &(fluidData[testInd*4+E_PTT_TER]);
				watVal = &(fluidData[testInd*4+E_PTT_WAT]);
				ideVal = &(extraData[testInd*4+E_PTT_STB]);
				bldVal = &(extraData[testInd*4+E_PTT_BLD]);
				
				
				isAir = 
					(*watVal == UNIT_MIN) &&
					(*terVal == UNIT_MIN) &&
					(*bldVal == UNIT_MIN);
					
				//terVal2 = &(fluidData[testInd2*4+E_PTT_TER]);
				//watVal2 = &(fluidData[testInd2*4+E_PTT_WAT]);
				// terBelow = 
				// 	//(*watVal2 == UNIT_MIN) &&
				// 	(*terVal2 == UNIT_MIN);
				// if (isAir&&terBelow) {
				// 	airCount++;
				// }
				
				if (
					(*ideVal == 0) &&
					(*watVal >= 0)
				) {
					foundInd = testInd;
				}
			}
			
			if (foundInd >= 0) {
				indexStack.push_back(foundInd);
			}
			else {
				indexStack.pop_back();
			}
			
		}
		
		
		maxSize = fsPtr->fluidIds.size();
		
		
		
		if (
			(emptyWaterCount == maxSize)
			// && (airCount == 0)	
		) {
			// all fluid cells in this plane are empty,
			// and there are no adjacent air cells,
			// so get rid of all empty water cells
			
			
			for (i = 0; i < maxSize; i++) {
				ind = fsPtr->fluidIds[i];
				
				fluidData[ind*4+E_PTT_WAT] = UNIT_MIN;
			}
			
			fsPtr->fluidIds.clear();
			
		}
		else {
			
			
			/*
			
			
			// equalize fluid density in this plane
			
			if (maxSize > 0) {
				
				totWat = 0;
				for (i = 0; i < maxSize; i++) {
					ind = fsPtr->fluidIds[i];
					watVal = &(fluidData[ind*4+E_PTT_WAT]);
					totWat += *watVal;
				}
				
				divWat = maxSize;
				if (divWat > 0) {
					
					targWat = (totWat/divWat);
					
					if ( targWat > 0 ) {
						
						totWat2 = 0;
						for (i = 0; i < maxSize; i++) {
							ind = fsPtr->fluidIds[i];
							watVal = &(fluidData[ind*4+E_PTT_WAT]);
							*watVal += (targWat - *watVal)/2;
							totWat2 += *watVal;
						}
						
						i = 0;
						
						while (totWat > totWat2) {
							
							ind = fsPtr->fluidIds[i];
							watVal = &(fluidData[ind*4+E_PTT_WAT]);
							
							*watVal += 1;
							totWat2 += 1;
							
							i++;
							if (i==maxSize) { i = 0; }
							
						}
						
						while (totWat < totWat2) {
							
							ind = fsPtr->fluidIds[i];
							watVal = &(fluidData[ind*4+E_PTT_WAT]);
							
							*watVal -= 1;
							totWat2 -= 1;
							
							i++;
							if (i==maxSize) { i = 0; }
						}
						
					}
				}
				
			}
			
			
			
			
			
			
			*/
			
			
			
		}
		
		//fsPlaneVec.pop_back();
		
		if ( fsPtr->fluidIds.size() == 0 ) {
			
			return false;
		}
		else {
			return true;
		}
		
		
		
	}
bool GameFluid::floodFillId (int startInd, int newId)
                                                  {
		
		indexStack.clear();
		indexStack.push_back(startInd);
		
		int dir;
		
		int ind;
		int testInd;
		int* bldVal;
		int* terVal;
		int* watVal;
		int* watVal2;
		int* ideVal;
		
		int i;
		int j;
		int k;
		int n;
		
		int* curVec;
		
		int foundInd;
		
		int testI;
		int testJ;
		int testK;
		
		bool notFound;
		bool isAir;
		bool isEmptyWater;
		
		fsVec.push_back(FluidStruct());
		
		
		FluidStruct* fsPtr = &(fsVec.back());
		
		
		fsPtr->minX = volSizePrimBuf*2;
		fsPtr->minY = volSizePrimBuf*2;
		fsPtr->minZ = volSizePrimBuf*2;
		fsPtr->maxX = -volSizePrimBuf*2;
		fsPtr->maxY = -volSizePrimBuf*2;
		fsPtr->maxZ = -volSizePrimBuf*2;
		
		while (indexStack.size() > 0) {
			
			ind = indexStack.back();
			extraData[ind*4+E_PTT_IDE] = newId;
			
			k = ind/(volSizePrimBuf*volSizePrimBuf);
			j = (ind - k*volSizePrimBuf*volSizePrimBuf)/volSizePrimBuf;
			i = ind - ( j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf );
			
			watVal2 = &(fluidData[ind*4+E_PTT_WAT]);
			
			
			
			notFound = true;
			if (*watVal2 < UNIT_MAX) {
				// if water is partially filled, add to fluidId stack
				fsPtr->fluidIds.push_back(ind);
				notFound = false;
			}
			else {
				// if its full and there is earth above, add it
				testInd = ind + volSizePrimBuf*volSizePrimBuf;
				
				if (
					(fluidData[testInd*4+E_PTT_TER] != UNIT_MIN) ||
					(extraData[testInd*4+E_PTT_BLD] != UNIT_MIN)	
				) {
					fsPtr->fluidIds.push_back(ind);
					notFound = false;
				}
				
			}
			
			//fsPtr->fluidIds.push_back(ind);
			
			foundInd = -1;
			
			
			for (dir = 0; dir < 6; dir++) {
				testI = (i+DIR_VECS_I[dir][0]);
				testJ = (j+DIR_VECS_I[dir][1]);
				testK = (k+DIR_VECS_I[dir][2]);
				testInd = 
					testI +
					testJ*volSizePrimBuf +
					testK*volSizePrimBuf*volSizePrimBuf;
				
				bldVal = &(extraData[testInd*4+E_PTT_BLD]);
				terVal = &(fluidData[testInd*4+E_PTT_TER]);
				watVal = &(fluidData[testInd*4+E_PTT_WAT]);
				ideVal = &(extraData[testInd*4+E_PTT_IDE]);
				
				isAir = 
					(*watVal == UNIT_MIN) &&
					(*terVal == UNIT_MIN) &&
					(*bldVal == UNIT_MIN);
				
				isEmptyWater = (*watVal == 0);
				
				// check if water touches air or an empty water cell, if so add to fluidId stack
				if (
					(isAir||isEmptyWater)	&&
					notFound
				) {
					fsPtr->fluidIds.push_back(ind);
					notFound = false;
				}
				
				if (*ideVal == 0) {
					if (isAir) {
						// it is an unmarked air value or empty water cell adjacent to a water cell
						
						fsPtr->fluidIds.push_back(testInd);
						*ideVal = newId;
						
					}
					
					if (*watVal >= 0) {
						foundInd = testInd;
					}
					
				}
			}
			
			if (foundInd >= 0) {
				indexStack.push_back(foundInd);
			}
			else {
				indexStack.pop_back();
			}
			
		}
		
		
		for (n = 0; n < fsPtr->fluidIds.size(); n++) {
			
			ind = fsPtr->fluidIds[n];
			
			k = ind/(volSizePrimBuf*volSizePrimBuf);
			j = (ind - k*volSizePrimBuf*volSizePrimBuf)/volSizePrimBuf;
			i = ind - ( j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf );
			
			bldVal = &(extraData[ind*4+E_PTT_BLD]);
			terVal = &(fluidData[ind*4+E_PTT_TER]);
			watVal = &(fluidData[ind*4+E_PTT_WAT]);
			
			// remove ids from any air values so they can be reused
			if (
				(*watVal == UNIT_MIN) &&
				(*terVal == UNIT_MIN) &&
				(*bldVal == UNIT_MIN)
			) {
				extraData[ind*4+E_PTT_IDE] = 0;
			}
			
			//fluidData[ind*4+E_PTT_EDG] = UNIT_MAX;
			
			
			if (i < fsPtr->minX) {fsPtr->minX = i;}
			if (j < fsPtr->minY) {fsPtr->minY = j;}
			if (k < fsPtr->minZ) {fsPtr->minZ = k;}
			if (i > fsPtr->maxX) {fsPtr->maxX = i;}
			if (j > fsPtr->maxY) {fsPtr->maxY = j;}
			if (k > fsPtr->maxZ) {fsPtr->maxZ = k;}
			
		}
		
		if ( fsPtr->fluidIds.size() == 0) {
			fsVec.pop_back();
			return false;
		}
		else {
			return true;
		}
		
	}
bool GameFluid::inBounds (int i, int j, int k)
                                           {
		return (
			(watchMinX >= 0) &&
			(i >= watchMinX) &&
			(i <= watchMaxX) &&
			(j >= watchMinY) &&
			(j <= watchMaxY) &&
			(k >= watchMinZ) &&
			(k <= watchMaxZ)
		);
	}
void GameFluid::modifyUnit (FIVector4 * fPixelWorldCoordsBase, int brushAction, int modType, int radius)
          {
		modStack.push_back(ModUnitStruct());
		modStack.back().basePos.copyFrom(fPixelWorldCoordsBase);
		modStack.back().brushAction = brushAction;
		modStack.back().modType = modType;
		modStack.back().radius = radius;
	}
bool GameFluid::roundBox (FIVector4 * absVecFromCenter, FIVector4 * innerBoxRad, FIVector4 * cornerDisThicknessPower)
          {
		
		//doTraceVecND("absVecFromCenter ", absVecFromCenter);
		//doTraceVecND("innerBoxRad ", innerBoxRad);
		
		
		FIVector4 newP;
		FIVector4 orig1;
		FIVector4 orig2;
		
		orig1.setFXYZ(0.0f,0.0f,0.0f);
		
		//vec3 newP = abs(max( absVecFromCenter-(box_dim.xyz),0.0));
		
		orig2.copyFrom(absVecFromCenter);
		orig2.addXYZRef(innerBoxRad,-1.0f);
		newP.maxXYZ(&orig2,&orig1);
		newP.absXYZ();
		
		float powX = cornerDisThicknessPower->getFZ();
		float powY = cornerDisThicknessPower->getFW();
		
		
		

		//newP.xy = pow(newP.xy, box_power.xx );
		newP.powXYZ(powX,powX,1.0f);		
		//newP.x = pow( newP.x + newP.y, 1.0/box_power.x );
		newP.setFX(newP[0] + newP[1]);
		newP.powXYZ(1.0/powX,1.0f,1.0f);

		//newP.xz = pow(newP.xz, box_power.yy );
		newP.powXYZ(powY,1.0f,powY);
		//newP.x = pow( newP.x + newP.z, 1.0/box_power.y );
		newP.setFX(newP[0] + newP[2]);
		newP.powXYZ(1.0/powY,1.0f,1.0f);
		
		//newP.setFX(newP.length());
		
		
		float dis = 
			//(newP[0]-cornerDisThicknessPower->getFX());
			max(
				(newP[0]-cornerDisThicknessPower->getFX()),
				( (cornerDisThicknessPower->getFX()-cornerDisThicknessPower->getFY())-newP[0] )	
			);
		
		//cout << newP[0] << " - " << cornerDisThicknessPower->getFX() << " = " << dis << "\n";
		
		return dis < 0.0f;

	}
void GameFluid::fillCurrentGeom ()
                               {
		
		int i;
		int j;
		int k;
		
		
		FIVector4 paramArrGeom[E_PRIMTEMP_LENGTH];
		
		for (i = 0; i < E_PRIMTEMP_LENGTH; i++) {
			paramArrGeom[i].setFXYZW(
				singleton->paramArrGeom[i*4+0],
				singleton->paramArrGeom[i*4+1],
				singleton->paramArrGeom[i*4+2],
				singleton->paramArrGeom[i*4+3]
			);
		}
		
		FIVector4 innerBoxRad;
		FIVector4 absVecFromCenter;
		//FIVector4 centerCoord;
		
		FIVector4 baseVec;
		baseVec.averageXYZ(
			&(paramArrGeom[E_PRIMTEMP_BOUNDSMIN]),
			&(paramArrGeom[E_PRIMTEMP_BOUNDSMAX])
		);
		baseVec.addXYZRef(&(singleton->geomPoints[0]));
		baseVec.addXYZ(0.0,0.0,singleton->geomOrigOffset);
		baseVec.addXYZRef(&(singleton->volMinInPixels),-1.0f);
		baseVec.addXYZ(1.0f);
		
		FIVector4 curCoord;
		
		FIVector4 newCoordMin;
		FIVector4 newCoordMax;
		newCoordMin.copyFrom(&baseVec);
		newCoordMax.copyFrom(&baseVec);
		
		// centerCoord.averageXYZ(
		// 	&(paramArrGeom[E_PRIMTEMP_BOUNDSMAX]),
		// 	&(paramArrGeom[E_PRIMTEMP_BOUNDSMIN])
		// );
		innerBoxRad.averageNegXYZ(
			&(paramArrGeom[E_PRIMTEMP_BOUNDSMAX]),
			&(paramArrGeom[E_PRIMTEMP_BOUNDSMIN])
		);
		
		innerBoxRad.addXYZ(paramArrGeom[E_PRIMTEMP_CORNERDIS].getFX(),-1.0f);
		
		newCoordMin.addXYZRef(&(paramArrGeom[E_PRIMTEMP_VISMIN]));
		newCoordMax.addXYZRef(&(paramArrGeom[E_PRIMTEMP_VISMAX]));
		
		//newCoordMin.addXYZ(2.0f,2.0f,2.0f);
		//newCoordMax.addXYZ(2.0f,2.0f,2.0f);
		
		int iMin = clamp(newCoordMin.getIX(), vspMin, vspMax);
		int iMax = clamp(newCoordMax.getIX(), vspMin, vspMax);
		int jMin = clamp(newCoordMin.getIY(), vspMin, vspMax);
		int jMax = clamp(newCoordMax.getIY(), vspMin, vspMax);
		int kMin = clamp(newCoordMin.getIZ(), vspMin, vspMax);
		int kMax = clamp(newCoordMax.getIZ(), vspMin, vspMax);
		
		int* bldVal;
		int* terVal;
		int* watVal;
		
		int ind;
		
		
		
		// cout << "cornerDis " << paramArrGeom[E_PRIMTEMP_CORNERDIS].getFX() << "\n";
		// doTraceVecND("E_PRIMTEMP_BOUNDSMIN ", &(paramArrGeom[E_PRIMTEMP_BOUNDSMIN]));
		// doTraceVecND("E_PRIMTEMP_BOUNDSMAX ", &(paramArrGeom[E_PRIMTEMP_BOUNDSMAX]));
		// doTraceVecND("innerBoxRad ", &innerBoxRad);
		
		for (k = kMin; k < kMax; k++) {
			for (j = jMin; j < jMax; j++) {
				for (i = iMin; i < iMax; i++) {
					
					curCoord.setFXYZ(i,j,k);
					curCoord.addXYZ(0.5f);
					absVecFromCenter.copyFrom(&curCoord);
					absVecFromCenter.addXYZRef(&baseVec,-1.0f);
					absVecFromCenter.absXYZ();
					
					if (roundBox(
						&absVecFromCenter,
						&innerBoxRad,
						&(paramArrGeom[E_PRIMTEMP_CORNERDIS])
					)) {
						
						
						ind = (i + j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf);
						
						bldVal = &(extraData[ind*4+E_PTT_BLD]);
						terVal = &(fluidData[ind*4+E_PTT_TER]);
						watVal = &(fluidData[ind*4+E_PTT_WAT]);
						
						*bldVal = UNIT_MAX;
						//*terVal = UNIT_MAX;
						*watVal = UNIT_MIN;
						
						
						
					}
					
					
				}
			}
		}
		
		
		
	}
void GameFluid::applyUnitModification (FIVector4 * fPixelWorldCoordsBase, int brushAction, int modType, int radius)
          {
		
			FIVector4 baseVec;
			baseVec.copyFrom(fPixelWorldCoordsBase);
			baseVec.addXYZRef(&(singleton->volMinInPixels),-1.0f);
			
			
			FIVector4 curCoord;
			
			FIVector4 newCoordMin;
			FIVector4 newCoordMax;
			newCoordMin.copyFrom(&baseVec);
			newCoordMax.copyFrom(&baseVec);
			
			
			newCoordMin.addXYZ(-radius + 1);
			newCoordMax.addXYZ( radius + 1);
			
			int iMin = clamp(newCoordMin.getIX(), vspMin, vspMax);
			int iMax = clamp(newCoordMax.getIX(), vspMin, vspMax);
			int jMin = clamp(newCoordMin.getIY(), vspMin, vspMax);
			int jMax = clamp(newCoordMax.getIY(), vspMin, vspMax);
			int kMin = clamp(newCoordMin.getIZ(), vspMin, vspMax);
			int kMax = clamp(newCoordMax.getIZ(), vspMin, vspMax);
			
			
			// cout << "applyUnitModification "
			// 	<< iMin << " " << iMax << " "
			// 	<< jMin << " " << jMax << " "
			// 	<< kMin << " " << kMax << "\n";
			
			
			int i;
			int j;
			int k;
			
			int* empVal;
			int* bldVal;
			int* terVal;
			int* watVal;
			// int* ideVal;
			// int* stbVal;
			
			int ind;
			
			for (k = kMin; k < kMax; k++) {
				for (j = jMin; j < jMax; j++) {
					for (i = iMin; i < iMax; i++) {
						
						curCoord.setFXYZ(i,j,k);
						
						if (baseVec.distance(&curCoord) <= radius) {
							
							
							ind = (i + j*volSizePrimBuf + k*volSizePrimBuf*volSizePrimBuf);
							
							bldVal = &(extraData[ind*4+E_PTT_BLD]);
							empVal = &(fluidData[ind*4+E_PTT_EMP]);
							terVal = &(fluidData[ind*4+E_PTT_TER]);
							watVal = &(fluidData[ind*4+E_PTT_WAT]);
							// ideVal = &(extraData[ind*4+E_PTT_IDE]);
							// stbVal = &(extraData[ind*4+E_PTT_STB]);
							
							switch (brushAction) {
								case E_BRUSH_MOVE:
									return;
								break;
								case E_BRUSH_ADD:
								
									if (modType == E_PTT_WAT) {
										if (*terVal == UNIT_MIN) {
											*watVal = UNIT_MAX;
										}
									}
									else {
										*terVal = UNIT_MAX;
										*watVal = UNIT_MIN;
									}
									
									
									
								break;
								case E_BRUSH_SUB:
									*empVal = UNIT_MAX;
									*bldVal = UNIT_MIN;
									*terVal = UNIT_MIN;
									*watVal = UNIT_MIN;
								break;
								case E_BRUSH_REF:
									watchMinX = iMin;
									watchMaxX = iMax;
									
									watchMinY = jMin;
									watchMaxY = jMax;
									
									watchMinZ = kMin;
									watchMaxZ = kMax;
									
								break;
								
							}
							
							
						}
						
						
					}
				}
			}
			
	}
#undef LZZ_INLINE
 
