<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Получаем данные из формы
    $recipient_email = $_POST["recipient_email"];
    $message = $_POST["message"];
    
    // Настройки для отправки письма
    $subject = "Новое сообщение от пользователя";
    $headers = "From: ваш_адрес_почты@example.com";
    
    // Отправляем письмо
    if (mail($recipient_email, $subject, $message, $headers)) {
        echo "Письмо успешно отправлено";
    } else {
        echo "Ошибка при отправке письма";
    }
}
?>
