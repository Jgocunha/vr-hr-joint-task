# Dynamic Neural Field Architecture


![DNF Architecture](dnf-architecture.png)


| Field                                 | Parameters                                                                                                                                                                                                                             |
| ------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Action Observation Field (AOL)**    | τ = 100, resting = -5, sigmoid (x_shift = 0, steepness = 4), Gauss kernel self (A = 1, σ = 1.5), noise = 0.001                                                                                                                         |                                                                                        |
| **Action Simulation Field (ASL)**     | τ = 100, resting = -5, sigmoid (0, 4), Lateral interactions (A_exc = 1, σ_exc = 2, A_inh = 0.5, σ_inh = 1.5, S_self = -0.1), Gauss kernel from AOL (A = 2.4, σ = 0.755), noise = 0.001                                                 |
| **Object Representation Field (ORL)** | τ = 100, resting = -5, sigmoid (0, 4), Gauss kernel self (A = 1, σ = 2), Gauss kernel to ASL (A = 1.9, σ = 0.7), noise = 0.001                                                                                                                                                                                                                                                         |
| **Action Execution Field (AEL)**      | τ = 100, resting = -5, sigmoid (0, 4), Lateral interactions (A_exc = 4.75, σ_exc = 8.143, A_inh = 3.375, σ_inh = 5.677, S_self = -2.5), Gauss kernel from ASL (A = 1, σ = -1.5), Gauss kernel from ORL (A = 2, σ = 1.5), noise = 0.001 |
