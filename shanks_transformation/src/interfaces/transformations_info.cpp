#include "../../include/interfaces/transformations_info.hpp"

ITransformationInfo::~ITransformationInfo() {}

transformation_id_t          AndersonAccelerationAlgorithmInfo::getId() const {return transformation_id_t::anderson_acceleration_algorithm_id;}
transformation_id_t           BrezinskiThetaTransformationInfo::getId() const {return transformation_id_t::brezinski_theta_transformation_id;}
transformation_id_t                ChangWynnTransformationInfo::getId() const {return transformation_id_t::chang_wynn_transformation_id;}
transformation_id_t               DrummondDUTransformationInfo::getId() const {return transformation_id_t::drummond_d_u_transformation_id;}
transformation_id_t               DrummondDTTransformationInfo::getId() const {return transformation_id_t::drummond_d_t_transformation_id;}
transformation_id_t           DrummondDTWaveTransformationInfo::getId() const {return transformation_id_t::drummond_d_t_wave_transformation_id;}
transformation_id_t               DrummondDVTransformationInfo::getId() const {return transformation_id_t::drummond_d_v_transformation_id;}
transformation_id_t           DrummondDVWaveTransformationInfo::getId() const {return transformation_id_t::drummond_d_v_wave_transformation_id;}
transformation_id_t               JTransformationAlgorithmInfo::getId() const {return transformation_id_t::j_transformation_id;}
transformation_id_t      RecurrentDrummondDUTransformationInfo::getId() const {return transformation_id_t::recurrent_drummond_d_u_transformation_id;}
transformation_id_t      RecurrentDrummondDTTransformationInfo::getId() const {return transformation_id_t::recurrent_drummond_d_t_transformation_id;}
transformation_id_t      RecurrentDrummondDVTransformationInfo::getId() const {return transformation_id_t::recurrent_drummond_d_v_transformation_id;}
transformation_id_t  RecurrentDrummondDTWaveTransformationInfo::getId() const {return transformation_id_t::recurrent_drummond_d_t_wave_transformation_id;}
transformation_id_t  RecurrentDrummondDVWaveTransformationInfo::getId() const {return transformation_id_t::recurrent_drummond_d_v_wave_transformation_id;}
transformation_id_t                FordSidi2TransformationInfo::getId() const {return transformation_id_t::ford_sidi_2_transformation_id;}
transformation_id_t                FordSidi3TransformationInfo::getId() const {return transformation_id_t::ford_sidi_3_transformation_id;}
transformation_id_t              LevinSidiLUTransformationInfo::getId() const {return transformation_id_t::levin_sidi_l_u_transformation_id;}
transformation_id_t              LevinSidiLTTransformationInfo::getId() const {return transformation_id_t::levin_sidi_l_t_wave_transformation_id;}
transformation_id_t          LevinSidiLTWaveTransformationInfo::getId() const {return transformation_id_t::levin_sidi_l_t_wave_transformation_id;}
transformation_id_t              LevinSidiLVTransformationInfo::getId() const {return transformation_id_t::levin_sidi_l_v_transformation_id;}
transformation_id_t          LevinSidiLVWaveTransformationInfo::getId() const {return transformation_id_t::levin_sidi_l_v_wave_transformation_id;}
transformation_id_t     RecurrentLevinSidiLUTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_l_u_transformation_id;}
transformation_id_t     RecurrentLevinSidiLTTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_l_t_transformation_id;}
transformation_id_t     RecurrentLevinSidiLVTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_l_v_transformation_id;}
transformation_id_t RecurrentLevinSidiLTWaveTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_l_t_wave_transformation_id;}
transformation_id_t RecurrentLevinSidiLVWaveTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_l_v_wave_transformation_id;}
transformation_id_t              LevinSidiMUTransformationInfo::getId() const {return transformation_id_t::levin_sidi_m_u_transformation_id;}
transformation_id_t              LevinSidiMTTransformationInfo::getId() const {return transformation_id_t::levin_sidi_m_t_transformation_id;}
transformation_id_t          LevinSidiMTWaveTransformationInfo::getId() const {return transformation_id_t::levin_sidi_m_t_wave_transformation_id;}
transformation_id_t              LevinSidiMVTransformationInfo::getId() const {return transformation_id_t::levin_sidi_m_v_transformation_id;}
transformation_id_t          LevinSidiMVWaveTransformationInfo::getId() const {return transformation_id_t::levin_sidi_m_v_wave_transformation_id;}
transformation_id_t              LevinSidiSUTransformationInfo::getId() const {return transformation_id_t::levin_sidi_s_u_transformation_id;}
transformation_id_t              LevinSidiSTTransformationInfo::getId() const {return transformation_id_t::levin_sidi_s_t_transformation_id;}
transformation_id_t          LevinSidiSTWaveTransformationInfo::getId() const {return transformation_id_t::levin_sidi_s_t_wave_transformation_id;}
transformation_id_t              LevinSidiSVTransformationInfo::getId() const {return transformation_id_t::levin_sidi_s_v_transformation_id;}
transformation_id_t          LevinSidiSVWaveTransformationInfo::getId() const {return transformation_id_t::levin_sidi_s_v_wave_transformation_id;}
transformation_id_t     RecurrentLevinSidiSUTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_s_u_transformation_id;}
transformation_id_t     RecurrentLevinSidiSTTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_s_t_transformation_id;}
transformation_id_t     RecurrentLevinSidiSVTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_s_v_transformation_id;}
transformation_id_t RecurrentLevinSidiSTWaveTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_s_t_wave_transformation_id;}
transformation_id_t RecurrentLevinSidiSVWaveTransformationInfo::getId() const {return transformation_id_t::recurrent_levin_sidi_s_v_wave_transformation_id;}
transformation_id_t                  LubkinWTransformationInfo::getId() const {return transformation_id_t::lubkin_w_transformation_id;}
transformation_id_t               RichardsonTransformationInfo::getId() const {return transformation_id_t::richardson_transformation_id;}
transformation_id_t                   ShanksTransformationInfo::getId() const {return transformation_id_t::shanks_transformation_id;}
transformation_id_t        ShanksAlternatingTransformationInfo::getId() const {return transformation_id_t::shanks_alternating_transformation_id;}
transformation_id_t                  WenigerTransformationInfo::getId() const {return transformation_id_t::weniger_transformation_id;}
transformation_id_t             WynnEpsilon1TransformationInfo::getId() const {return transformation_id_t::wynn_epsilon_1_transformation_id;}
transformation_id_t             WynnEpsilon2TransformationInfo::getId() const {return transformation_id_t::wynn_epsilon_2_transformation_id;}
transformation_id_t             WynnEpsilon3TransformationInfo::getId() const {return transformation_id_t::wynn_epsilon_3_transformation_id;}
transformation_id_t               WynnRhoRhoTransformationInfo::getId() const {return transformation_id_t::wynn_rho_gamma_rho_transformation_id;}
transformation_id_t       WynnRhoGeneralizedTransformationInfo::getId() const {return transformation_id_t::wynn_rho_generalized_transformation_id;}
transformation_id_t          WynnRhoGammaRhoTransformationInfo::getId() const {return transformation_id_t::wynn_rho_gamma_rho_transformation_id;}

std::string          AndersonAccelerationAlgorithmInfo::getName() const {return "Anderson's acceleration algorithm";}
std::string           BrezinskiThetaTransformationInfo::getName() const {return "Brezinski Theta Transformation";}
std::string                ChangWynnTransformationInfo::getName() const {return "Chang-Wynn Transformation";}
std::string               DrummondDUTransformationInfo::getName() const {return "Drummond D-U Transformation";}
std::string               DrummondDTTransformationInfo::getName() const {return "Drummond D-T Transformation";}
std::string           DrummondDTWaveTransformationInfo::getName() const {return "Drummond D-T Wave Transformation";}
std::string               DrummondDVTransformationInfo::getName() const {return "Drummond D-V Transformation";}
std::string           DrummondDVWaveTransformationInfo::getName() const {return "Drummond D-V Wave Transformation";}
std::string               JTransformationAlgorithmInfo::getName() const {return "J transformation";}
std::string      RecurrentDrummondDUTransformationInfo::getName() const {return "Recurrent Drummond D-U Transformation";}
std::string      RecurrentDrummondDTTransformationInfo::getName() const {return "Recurrent Drummond D-T Transformation";}
std::string      RecurrentDrummondDVTransformationInfo::getName() const {return "Recurrent Drummond D-V Transformation";}
std::string  RecurrentDrummondDTWaveTransformationInfo::getName() const {return "Recurrent Drummond D-T Wave Transformation";}
std::string  RecurrentDrummondDVWaveTransformationInfo::getName() const {return "Recurrent Drummond D-V Wave Transformation";}
std::string                FordSidi2TransformationInfo::getName() const {return "Ford Sidi-2 Transformation";}
std::string                FordSidi3TransformationInfo::getName() const {return "Ford Sidi-3 Transformation";}
std::string              LevinSidiLUTransformationInfo::getName() const {return "Levin Sidi-L U Transformation";}
std::string              LevinSidiLTTransformationInfo::getName() const {return "Levin Sidi-L T Transformation";}
std::string          LevinSidiLTWaveTransformationInfo::getName() const {return "Levin Sidi-L T Wave Transformation";}
std::string              LevinSidiLVTransformationInfo::getName() const {return "Levin Sidi-L V Transformation";}
std::string          LevinSidiLVWaveTransformationInfo::getName() const {return "Levin Sidi-L V Wave Transformation";}
std::string     RecurrentLevinSidiLUTransformationInfo::getName() const {return "Recurrent Levin Sidi-L U Transformation";}
std::string     RecurrentLevinSidiLTTransformationInfo::getName() const {return "Recurrent Levin Sidi-L T Transformation";}
std::string     RecurrentLevinSidiLVTransformationInfo::getName() const {return "Recurrent Levin Sidi-L V Transformation";}
std::string RecurrentLevinSidiLTWaveTransformationInfo::getName() const {return "Recurrent Levin Sidi-L T Wave Transformation";}
std::string RecurrentLevinSidiLVWaveTransformationInfo::getName() const {return "Recurrent Levin Sidi-L V Wave Transformation";}
std::string              LevinSidiMUTransformationInfo::getName() const {return "Levin Sidi-M U Transformation";}
std::string              LevinSidiMTTransformationInfo::getName() const {return "Levin Sidi-M T Transformation";}
std::string          LevinSidiMTWaveTransformationInfo::getName() const {return "Levin Sidi-M T Wave Transformation";}
std::string              LevinSidiMVTransformationInfo::getName() const {return "Levin Sidi-M V Transformation";}
std::string          LevinSidiMVWaveTransformationInfo::getName() const {return "Levin Sidi-M V Wave Transformation";}
std::string              LevinSidiSUTransformationInfo::getName() const {return "Levin Sidi-S U Transformation";}
std::string              LevinSidiSTTransformationInfo::getName() const {return "Levin Sidi-S T Transformation";}
std::string          LevinSidiSTWaveTransformationInfo::getName() const {return "Levin Sidi-S T Wave Transformation";}
std::string              LevinSidiSVTransformationInfo::getName() const {return "Levin Sidi-S V Transformation";}
std::string          LevinSidiSVWaveTransformationInfo::getName() const {return "Levin Sidi-S V Wave Transformation";}
std::string     RecurrentLevinSidiSUTransformationInfo::getName() const {return "Recurrent Levin Sidi-S U Transformation";}
std::string     RecurrentLevinSidiSTTransformationInfo::getName() const {return "Recurrent Levin Sidi-S T Transformation";}
std::string     RecurrentLevinSidiSVTransformationInfo::getName() const {return "Recurrent Levin Sidi-S V Transformation";}
std::string RecurrentLevinSidiSTWaveTransformationInfo::getName() const {return "Recurrent Levin Sidi-S T Wave Transformation";}
std::string RecurrentLevinSidiSVWaveTransformationInfo::getName() const {return "Recurrent Levin Sidi-S V Wave Transformation";}
std::string                  LubkinWTransformationInfo::getName() const {return "Lubkin W Transformation";}
std::string               RichardsonTransformationInfo::getName() const {return "Richardson Transformation";}
std::string                   ShanksTransformationInfo::getName() const {return "Shanks Transformation";}
std::string        ShanksAlternatingTransformationInfo::getName() const {return "Shanks Alternating Transformation";}
std::string                  WenigerTransformationInfo::getName() const {return "Weniger Transformation";}
std::string             WynnEpsilon1TransformationInfo::getName() const {return "Wynn Epsilon-1 Transformation";}
std::string             WynnEpsilon2TransformationInfo::getName() const {return "Wynn Epsilon-2 Transformation";}
std::string             WynnEpsilon3TransformationInfo::getName() const {return "Wynn Epsilon-3 Transformation";}
std::string               WynnRhoRhoTransformationInfo::getName() const {return "Wynn Rho-Rho Transformation";}
std::string       WynnRhoGeneralizedTransformationInfo::getName() const {return "Wynn Rho Generalized Transformation";}
std::string          WynnRhoGammaRhoTransformationInfo::getName() const {return "Wynn Rho-Gamma-Rho Transformation";}
