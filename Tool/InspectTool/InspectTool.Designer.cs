namespace InspectTool
{
    partial class InspectTool
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.LocationType = new System.Windows.Forms.ComboBox();
            this.ServiceType = new System.Windows.Forms.ComboBox();
            this.UpdateBtn = new System.Windows.Forms.Button();
            this.Check_Inspect = new System.Windows.Forms.CheckBox();
            this.NoticeText = new System.Windows.Forms.TextBox();
            this.TitleText = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.ReloadBtn = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // LocationType
            // 
            this.LocationType.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.LocationType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.LocationType.FormattingEnabled = true;
            this.LocationType.Location = new System.Drawing.Point(238, 44);
            this.LocationType.Name = "LocationType";
            this.LocationType.Size = new System.Drawing.Size(120, 20);
            this.LocationType.TabIndex = 0;
            this.LocationType.SelectedIndexChanged += new System.EventHandler(this.LocationType_SelectedIndexChanged);
            // 
            // ServiceType
            // 
            this.ServiceType.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.ServiceType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ServiceType.FormattingEnabled = true;
            this.ServiceType.Location = new System.Drawing.Point(238, 18);
            this.ServiceType.Name = "ServiceType";
            this.ServiceType.Size = new System.Drawing.Size(120, 20);
            this.ServiceType.TabIndex = 1;
            this.ServiceType.SelectedIndexChanged += new System.EventHandler(this.ServiceType_SelectedIndexChanged);
            // 
            // UpdateBtn
            // 
            this.UpdateBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.UpdateBtn.Location = new System.Drawing.Point(407, 313);
            this.UpdateBtn.Name = "UpdateBtn";
            this.UpdateBtn.Size = new System.Drawing.Size(96, 23);
            this.UpdateBtn.TabIndex = 2;
            this.UpdateBtn.Text = "Update";
            this.UpdateBtn.UseVisualStyleBackColor = true;
            this.UpdateBtn.Click += new System.EventHandler(this.UpdateBtn_Click);
            // 
            // Check_Inspect
            // 
            this.Check_Inspect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.Check_Inspect.AutoSize = true;
            this.Check_Inspect.Font = new System.Drawing.Font("굴림", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Check_Inspect.Location = new System.Drawing.Point(381, 28);
            this.Check_Inspect.Name = "Check_Inspect";
            this.Check_Inspect.Size = new System.Drawing.Size(122, 31);
            this.Check_Inspect.TabIndex = 3;
            this.Check_Inspect.Text = "Inspect";
            this.Check_Inspect.UseVisualStyleBackColor = true;
            // 
            // NoticeText
            // 
            this.NoticeText.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.NoticeText.Location = new System.Drawing.Point(15, 134);
            this.NoticeText.Multiline = true;
            this.NoticeText.Name = "NoticeText";
            this.NoticeText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.NoticeText.Size = new System.Drawing.Size(488, 173);
            this.NoticeText.TabIndex = 4;
            // 
            // TitleText
            // 
            this.TitleText.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.TitleText.Location = new System.Drawing.Point(15, 81);
            this.TitleText.Name = "TitleText";
            this.TitleText.Size = new System.Drawing.Size(488, 21);
            this.TitleText.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("굴림", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(13, 62);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(37, 16);
            this.label1.TabIndex = 6;
            this.label1.Text = "Title";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("굴림", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label2.Location = new System.Drawing.Point(13, 111);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(75, 16);
            this.label2.TabIndex = 7;
            this.label2.Text = "Message";
            // 
            // ReloadBtn
            // 
            this.ReloadBtn.Location = new System.Drawing.Point(15, 18);
            this.ReloadBtn.Name = "ReloadBtn";
            this.ReloadBtn.Size = new System.Drawing.Size(94, 23);
            this.ReloadBtn.TabIndex = 8;
            this.ReloadBtn.Text = "Reload";
            this.ReloadBtn.UseVisualStyleBackColor = true;
            this.ReloadBtn.Click += new System.EventHandler(this.ReloadBtn_Click);
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(152, 18);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(80, 12);
            this.label3.TabIndex = 9;
            this.label3.Text = "Service Type";
            // 
            // label4
            // 
            this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(174, 47);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 12);
            this.label4.TabIndex = 10;
            this.label4.Text = "Location";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("굴림", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label5.ForeColor = System.Drawing.Color.Red;
            this.label5.Location = new System.Drawing.Point(85, 111);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(158, 16);
            this.label5.TabIndex = 11;
            this.label5.Text = "(Max Line Count : 3)";
            // 
            // InspectTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(515, 348);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.ReloadBtn);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.TitleText);
            this.Controls.Add(this.NoticeText);
            this.Controls.Add(this.Check_Inspect);
            this.Controls.Add(this.UpdateBtn);
            this.Controls.Add(this.ServiceType);
            this.Controls.Add(this.LocationType);
            this.Name = "InspectTool";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "InspectTool";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox LocationType;
        private System.Windows.Forms.ComboBox ServiceType;
        private System.Windows.Forms.Button UpdateBtn;
        private System.Windows.Forms.CheckBox Check_Inspect;
        private System.Windows.Forms.TextBox NoticeText;
        private System.Windows.Forms.TextBox TitleText;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button ReloadBtn;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
    }
}

