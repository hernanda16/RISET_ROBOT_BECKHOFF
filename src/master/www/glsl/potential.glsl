uniform float max_field_radius;
uniform float max_field_gradient;

float _max_field_radius = max_field_radius;
float _max_field_gradient = max_field_gradient;

float _c = _max_field_gradient / (2.0 * _max_field_radius);
float _a = _max_field_gradient;
float _b = _c * _max_field_radius * _max_field_radius - _max_field_radius;

void updateVectorAttractive(float x1, float y1, float x2, float y2, out float r, out float theta)
{
    float distanceBetweenPoints = distance(vec2(x2, y2), vec2(x1, y1));
    float angleBetweenPoints = atan(y2 - y1, x2 - x1);

    if (distanceBetweenPoints <= _max_field_radius)
        r = 2.0 * _c * distanceBetweenPoints;
    else
        r = _a;

    theta = angleBetweenPoints;
}
