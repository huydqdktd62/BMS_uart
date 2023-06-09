/*
 * battery_model.c
 *
 *  Created on: Feb 6, 2023
 *      Author: ADMIN
 */

#include "../soc_ukf/battery_model.h"

#include "math_util.h"

static const float soc_lut_values[MODEL_SOC_LUT_SIZE] = { 0.005f, 0.010f,
		0.015f, 0.020f, 0.025f, 0.030f, 0.035f, 0.040f, 0.045f, 0.050f, 0.055f,
		0.060f, 0.065f, 0.070f, 0.075f, 0.080f, 0.085f, 0.090f, 0.095f, 0.100f,
		0.105f, 0.110f, 0.115f, 0.120f, 0.125f, 0.130f, 0.135f, 0.140f, 0.145f,
		0.150f, 0.155f, 0.160f, 0.165f, 0.170f, 0.175f, 0.180f, 0.185f, 0.190f,
		0.195f, 0.200f, 0.205f, 0.210f, 0.215f, 0.220f, 0.225f, 0.230f, 0.235f,
		0.240f, 0.245f, 0.250f, 0.255f, 0.260f, 0.265f, 0.270f, 0.275f, 0.280f,
		0.285f, 0.290f, 0.295f, 0.300f, 0.305f, 0.310f, 0.315f, 0.320f, 0.325f,
		0.330f, 0.335f, 0.340f, 0.345f, 0.350f, 0.355f, 0.360f, 0.365f, 0.370f,
		0.375f, 0.380f, 0.385f, 0.390f, 0.395f, 0.400f, 0.405f, 0.410f, 0.415f,
		0.420f, 0.425f, 0.430f, 0.435f, 0.440f, 0.445f, 0.450f, 0.455f, 0.460f,
		0.465f, 0.470f, 0.475f, 0.480f, 0.485f, 0.490f, 0.495f, 0.500f, 0.505f,
		0.510f, 0.515f, 0.520f, 0.525f, 0.530f, 0.535f, 0.540f, 0.545f, 0.550f,
		0.555f, 0.560f, 0.565f, 0.570f, 0.575f, 0.580f, 0.585f, 0.590f, 0.595f,
		0.600f, 0.605f, 0.610f, 0.615f, 0.620f, 0.625f, 0.630f, 0.635f, 0.640f,
		0.645f, 0.650f, 0.655f, 0.660f, 0.665f, 0.670f, 0.675f, 0.680f, 0.685f,
		0.690f, 0.695f, 0.700f, 0.705f, 0.710f, 0.715f, 0.720f, 0.725f, 0.730f,
		0.735f, 0.740f, 0.745f, 0.750f, 0.755f, 0.760f, 0.765f, 0.770f, 0.775f,
		0.780f, 0.785f, 0.790f, 0.795f, 0.800f, 0.805f, 0.810f, 0.815f, 0.820f,
		0.825f, 0.830f, 0.835f, 0.840f, 0.845f, 0.850f, 0.855f, 0.860f, 0.865f,
		0.870f, 0.875f, 0.880f, 0.885f, 0.890f, 0.895f, 0.900f, 0.905f, 0.910f,
		0.915f, 0.920f, 0.925f, 0.930f, 0.935f, 0.940f, 0.945f, 0.950f, 0.955f,
		0.960f, 0.965f, 0.970f, 0.975f, 0.980f, 0.985f, 0.990f, 0.995f, 0.9975f,
		1.0f };

static const float ocv_lut_values[MODEL_OCV_LUT_SIZE] = { 3.2045f, 3.2224f,
		3.2390f, 3.2544f, 3.2688f, 3.2827f, 3.2962f, 3.3092f, 3.3219f, 3.3337f,
		3.3444f, 3.3539f, 3.3626f, 3.3705f, 3.3779f, 3.3851f, 3.3919f, 3.3985f,
		3.4051f, 3.4114f, 3.4178f, 3.4242f, 3.4312f, 3.4378f, 3.4434f, 3.4484f,
		3.4530f, 3.4575f, 3.4619f, 3.4661f, 3.4703f, 3.4744f, 3.4786f, 3.4829f,
		3.4874f, 3.4919f, 3.4968f, 3.5019f, 3.5072f, 3.5124f, 3.5176f, 3.5227f,
		3.5279f, 3.5329f, 3.5381f, 3.5433f, 3.5485f, 3.5535f, 3.5582f, 3.5627f,
		3.5669f, 3.5710f, 3.5750f, 3.5790f, 3.5827f, 3.5866f, 3.5905f, 3.5942f,
		3.5980f, 3.6017f, 3.6054f, 3.6090f, 3.6124f, 3.6159f, 3.6192f, 3.6225f,
		3.6257f, 3.6288f, 3.6319f, 3.6350f, 3.6382f, 3.6413f, 3.6444f, 3.6475f,
		3.6507f, 3.6539f, 3.6570f, 3.6603f, 3.6637f, 3.6670f, 3.6704f, 3.6739f,
		3.6775f, 3.6811f, 3.6849f, 3.6886f, 3.6926f, 3.6965f, 3.7005f, 3.7047f,
		3.7089f, 3.7131f, 3.7174f, 3.7217f, 3.7262f, 3.7306f, 3.7351f, 3.7396f,
		3.7441f, 3.7486f, 3.7530f, 3.7574f, 3.7619f, 3.7664f, 3.7708f, 3.7753f,
		3.7800f, 3.7846f, 3.7894f, 3.7941f, 3.7990f, 3.8040f, 3.8092f, 3.8144f,
		3.8196f, 3.8249f, 3.8303f, 3.8356f, 3.8409f, 3.8461f, 3.8512f, 3.8562f,
		3.8612f, 3.8660f, 3.8707f, 3.8752f, 3.8795f, 3.8834f, 3.8874f, 3.8909f,
		3.8945f, 3.8979f, 3.9012f, 3.9045f, 3.9077f, 3.9109f, 3.9141f, 3.9175f,
		3.9209f, 3.9244f, 3.9281f, 3.9319f, 3.9358f, 3.9399f, 3.9441f, 3.9486f,
		3.9532f, 3.9581f, 3.9631f, 3.9683f, 3.9737f, 3.9792f, 3.9847f, 3.9905f,
		3.9963f, 4.0020f, 4.0078f, 4.0135f, 4.0192f, 4.0247f, 4.0300f, 4.0353f,
		4.0402f, 4.0451f, 4.0497f, 4.0542f, 4.0582f, 4.0621f, 4.0655f, 4.0685f,
		4.0710f, 4.0730f, 4.0747f, 4.0762f, 4.0775f, 4.0787f, 4.0800f, 4.0811f,
		4.0825f, 4.0837f, 4.0849f, 4.0863f, 4.0878f, 4.0893f, 4.0909f, 4.0927f,
		4.0945f, 4.0966f, 4.0988f, 4.1012f, 4.1041f, 4.1072f, 4.1106f, 4.1145f,
		4.1188f, 4.1237f, 4.1294f, 4.1357f, 4.1429f, 4.1512f, 4.1607f };

static const float derivative_lut_values[MODEL_DERIVATIVE_LUT_SIZE] = { 3.5812f,
		3.3162f, 3.0812f, 2.8937f, 2.7662f, 2.7074f, 2.5949f, 2.5362f, 2.3675f,
		2.1399f, 1.9050f, 1.7287f, 1.5949f, 1.4775f, 1.4312f, 1.3612f, 1.3175f,
		1.3162f, 1.2649f, 1.2887f, 1.2787f, 1.3912f, 1.3224f, 1.1200f, 0.9949f,
		0.9274f, 0.9050f, 0.8637f, 0.8562f, 0.8287f, 0.8312f, 0.8324f, 0.8575f,
		0.9050f, 0.8999f, 0.9825f, 1.0112f, 1.0550f, 1.0512f, 1.0337f, 1.0250f,
		1.0375f, 1.0099f, 1.0362f, 1.0449f, 1.0262f, 0.9987f, 0.9437f, 0.9024f,
		0.8487f, 0.8149f, 0.7962f, 0.8000f, 0.7474f, 0.7787f, 0.7687f, 0.7399f,
		0.7737f, 0.7350f, 0.7287f, 0.7212f, 0.6850f, 0.6924f, 0.6712f, 0.6499f,
		0.6387f, 0.6350f, 0.6224f, 0.6199f, 0.6237f, 0.6325f, 0.6099f, 0.6237f,
		0.6462f, 0.6262f, 0.6300f, 0.6512f, 0.6825f, 0.6600f, 0.6912f, 0.6862f,
		0.7287f, 0.7100f, 0.7712f, 0.7362f, 0.7937f, 0.7874f, 0.8087f, 0.8225f,
		0.8437f, 0.8350f, 0.8662f, 0.8699f, 0.8925f, 0.8812f, 0.8900f, 0.9112f,
		0.8950f, 0.9049f, 0.8850f, 0.8812f, 0.8975f, 0.8987f, 0.8774f, 0.9062f,
		0.9300f, 0.9212f, 0.9549f, 0.9462f, 0.9737f, 1.0012f, 1.0474f, 1.0337f,
		1.0524f, 1.0575f, 1.0712f, 1.0599f, 1.0550f, 1.0475f, 1.0237f, 0.9875f,
		1.0087f, 0.9687f, 0.9287f, 0.9024f, 0.8587f, 0.7887f, 0.7899f, 0.7100f,
		0.7212f, 0.6712f, 0.6687f, 0.6537f, 0.6387f, 0.6387f, 0.6487f, 0.6737f,
		0.6775f, 0.7050f, 0.7299f, 0.7624f, 0.7774f, 0.8300f, 0.8399f, 0.8987f,
		0.9174f, 0.9850f, 0.9949f, 1.0399f, 1.0762f, 1.0950f, 1.1149f, 1.1600f,
		1.1450f, 1.1474f, 1.1512f, 1.1562f, 1.1300f, 1.1037f, 1.0624f, 1.0550f,
		0.9887f, 0.9787f, 0.9187f, 0.8924f, 0.8112f, 0.7662f, 0.6887f, 0.6000f,
		0.4912f, 0.4087f, 0.3387f, 0.2950f, 0.2687f, 0.2425f, 0.2425f, 0.2312f,
		0.2674f, 0.2525f, 0.2387f, 0.2799f, 0.2887f, 0.3125f, 0.3162f, 0.3674f,
		0.3612f, 0.4037f, 0.4575f, 0.4762f, 0.5724f, 0.6287f, 0.6800f, 0.7637f,
		0.8700f, 0.9800f, 1.1324f, 1.2600f, 1.4474f, 3.3299f, 3.7825f, 3.7825f };

float get_soc_from_ocv(float ocv) {
	uint16_t index;
	float OCV = ocv;
	if (OCV < ocv_lut_values[0])
		OCV = ocv_lut_values[0];
	if (OCV > ocv_lut_values[MODEL_SOC_LUT_SIZE - 1])
		OCV = ocv_lut_values[MODEL_SOC_LUT_SIZE - 1];
	for (index = 0; index < MODEL_OCV_LUT_SIZE; index++) {
		if (OCV <= ocv_lut_values[index]) {
			break;
		}
	}
	return soc_lut_values[index];
}

float get_ocv_from_soc(float soc) {
	int32_t index = 0;
	if (soc < soc_lut_values[0])
		soc = soc_lut_values[0];
	if (soc > soc_lut_values[MODEL_SOC_LUT_SIZE - 1])
		soc = soc_lut_values[MODEL_SOC_LUT_SIZE - 1];
	index = binary_search_f(soc, soc_lut_values, MODEL_SOC_LUT_SIZE);
	return ocv_lut_values[index];
}

float get_ratio_from_soc(float soc) {
	uint16_t i;
	float docv;
	float ratio = 0.0f;
	float SOC;
	SOC = soc;
	if (SOC < soc_lut_values[0]) {
		SOC = soc_lut_values[0];
	}
	if (SOC > soc_lut_values[MODEL_OCV_LUT_SIZE - 1]) {
		SOC = soc_lut_values[MODEL_OCV_LUT_SIZE - 1];
	}
	for (i = 0; i < MODEL_SOC_LUT_SIZE; i++) {
		if (SOC <= soc_lut_values[i]) {
			docv = (SOC - soc_lut_values[i]) * derivative_lut_values[i];
			ratio = (ocv_lut_values[i] + docv) / SOC;
			break;
		}
	}
	return ratio;
}
